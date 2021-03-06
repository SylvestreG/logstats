//
// Created by sylvestre on 06/03/2021.
//
#include "buffer_splitter.h"
#include <spdlog/spdlog.h>

BufferSplitter::BufferSplitter() : _stopped{true} {}

void BufferSplitter::pushIntoSplitter(
    std::vector<std::pair<Timepoint, std::string>> &&buffers) {
  std::unique_lock<std::mutex> lock(_buffer._bufferMtx);
  for (auto &value : buffers) {
    _buffer._bufferMap.emplace(value);
  }
  _buffer._bufferCv.notify_one();
  buffers.clear();
}

void BufferSplitter::start() {
  if (_stopped) {
    _stopped = false;
    _splitterThread = std::thread([&]() { split(); });
  }
}

void BufferSplitter::stop() {
  _stopped = true;
  _splitterThread.join();
}

void BufferSplitter::split() {
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
  }
}

std::map<Timepoint, std::string> BufferSplitter::getWorkingMap() {
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