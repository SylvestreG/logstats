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
#ifndef LOGSTATS_BUFFER_SPLITTER_H
#define LOGSTATS_BUFFER_SPLITTER_H

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace clf {

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

struct BufferResouces {
  std::mutex _bufferMtx;
  std::condition_variable _bufferCv;
  std::map<Timepoint, std::string> _bufferMap;

  std::string _leftover;
};

using newLineCallBack =
    std::function<void(std::pair<clf::Timepoint, std::string> &&)>;

class BufferSplitter {
public:
  BufferSplitter();
  BufferSplitter(BufferSplitter const &) = delete;
  ~BufferSplitter() = default;

  BufferSplitter &operator=(BufferSplitter const &) = delete;

  void setNewLineCallback(newLineCallBack cb);

  void start(); // run splitter thread
  void stop();  // stop splitter thread

  // push some work for splitter thread
  void
  pushIntoSplitter(std::vector<std::pair<Timepoint, std::string>> &&buffers);

private:
  void split();

  // The aim of this helper is to give to consume
  // in _buffer all valid line and return it.
  // It *MUST* be call when _buffer mutex is locked
  // because is access heavily to _buffer that can
  // be access in other threads in pushIntoSplitter.
  std::map<Timepoint, std::string> getWorkingMap();
  void consumeBuffers(std::map<Timepoint, std::string> &&map);

  newLineCallBack _newLineCb;
  bool _stopped;
  std::thread _splitterThread;
  BufferResouces _buffer;
};
} // namespace clf

#endif // LOGSTATS_BUFFER_SPLITTER_H
