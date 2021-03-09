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
#ifndef __FILE_WATCHER_H__
#define __FILE_WATCHER_H__

#include <atomic>
#include <filesystem>
#include <functional>
#include <thread>

#include <libfswatch/c++/monitor.hpp>

#include "config.h"

namespace clf {

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;
using newBufferCb =
    std::function<void(std::vector<std::pair<Timepoint, std::string>> &&)>;

class FileWatcher {
public:
  FileWatcher(std::shared_ptr<clf::Config>, const std::filesystem::path &);
  ~FileWatcher();

  FileWatcher() = delete;
  FileWatcher(FileWatcher const &) = delete;

  void setNewBufferCallback(newBufferCb cb);

  FileWatcher &operator=(FileWatcher const &) = delete;

  void start();
  void stop();

private:
  void onWrite();

  std::shared_ptr<clf::Config> _cfg;
  fsw::monitor *_monitor;
  std::thread _fileWatchThread;
  newBufferCb _buffCb;
  int _fd;
};
} // namespace clf

#endif //__FILE_WATCHER_H__