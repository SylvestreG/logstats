/*
 * Copyright (c) 2021 Sylvestre Gallon
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "buffer_splitter.h"
#include <spdlog/spdlog.h>

clf::BufferSplitter::BufferSplitter() : _stopped{true} {}

void clf::BufferSplitter::pushIntoSplitter(
    std::vector<std::pair<clf::Timepoint, std::string>> &&buffers) {
  std::unique_lock<std::mutex> lock(_buffer._bufferMtx);
  for (auto &value : buffers) {
    auto it = _buffer._bufferMap.find(value.first);
    if (it == _buffer._bufferMap.end())
      _buffer._bufferMap.emplace(value);
    else
      it->second.append(value.second);
  }
  _buffer._bufferCv.notify_one();
  buffers.clear();
}

void clf::BufferSplitter::start() {
  if (_stopped) {
    _stopped = false;
    _splitterThread = std::thread([&]() { split(); });
  }
}

void clf::BufferSplitter::stop() {
  {
    std::unique_lock<std::mutex> lck(_buffer._bufferMtx);
    _stopped = true;
    _buffer._bufferCv.notify_one();
  }
  _splitterThread.join();
}

void clf::BufferSplitter::split() {
  while (!_stopped) {
    // wakeup on terminate asked or
    // on new data into buffer;
    std::unique_lock<std::mutex> lck(_buffer._bufferMtx);

    _buffer._bufferCv.wait(lck, [&]() -> bool {
      if (_stopped)
        return true;

      return !_buffer._bufferMap.empty();
    });

    if (_stopped)
      return;

    auto &&copyMap = getWorkingMap();

    // we have finish with buffer queue, it is now time to parse !!!
    lck.unlock();

    // split data and call the pool to parse it !
    if (!copyMap.empty())
      consumeBuffers(std::move(copyMap));
  }
}

std::map<clf::Timepoint, std::string> clf::BufferSplitter::getWorkingMap() {

  // if there is some leftover put it in first buffer
  if (!_buffer._leftover.empty()) {
    _buffer._bufferMap.begin()->second.insert(0, _buffer._leftover);
    _buffer._leftover.clear();
  }

  auto last = _buffer._bufferMap.rbegin();
  std::map<Timepoint, std::string> mapCopy;

  // the last character of last buffer is \n
  // we can process all saved data;
  if (*last->second.rbegin() == '\n') {
    mapCopy = std::move(_buffer._bufferMap);
    _buffer._bufferMap.clear();

    return mapCopy;
  }

  // looking for a newline in last buffer.
  auto posLastNewLine = last->second.rfind("\n");
  if (posLastNewLine != std::string::npos) {
    // need to keep only all valid line from bufferQueue;
    std::string validLine = std::move(last->second.substr(0, posLastNewLine));
    validLine.substr(0, posLastNewLine);

    _buffer._leftover = std::move(last->second);
    _buffer._leftover.erase(posLastNewLine,
                            _buffer._leftover.size() - posLastNewLine);

    // copy and update last entry
    auto idx = last->first;
    mapCopy = std::move(_buffer._bufferMap);
    mapCopy[idx] = std::move(validLine);

    _buffer._bufferMap.clear();

    return mapCopy;
  }

  // no new line in the last entry.
  // they are two choice right now :
  // - first there is just one line (so no line to parser yet..
  // - append current line in previous one and relaunch getWorkingMap
  //   recursively
  if (_buffer._bufferMap.size() == 1)
    return std::map<Timepoint, std::string>();

  auto key = last->first;
  auto value = last->second;
  // remove last entry
  _buffer._bufferMap.erase(key);

  // merge n and n-1
  auto key2 = last->first;
  value.insert(0, _buffer._bufferMap.rbegin()->second);
  _buffer._bufferMap.erase(key2);
  _buffer._bufferMap.emplace(key, std::move(value));

  return getWorkingMap();
}

void clf::BufferSplitter::consumeBuffers(
    std::map<Timepoint, std::string> &&map) {
  std::string leftover;

  for (auto &entry : map) {
    if (!leftover.empty()) {
      entry.second.insert(0, std::move(leftover));
      leftover.clear();
    }

    auto pos = entry.second.find('\n');
    while (pos != std::string::npos) {
      std::string validStr = entry.second.substr(0, pos + 1);
      entry.second.erase(0, pos + 1);
      pos = entry.second.find('\n');
      if (_newLineCb)
        _newLineCb({entry.first, std::move(validStr)});
    }

    if (!entry.second.empty())
      leftover = std::move(entry.second);
  }

  map.clear();
}

void clf::BufferSplitter::setNewLineCallback(newLineCallBack cb) {
  _newLineCb = cb;
}
