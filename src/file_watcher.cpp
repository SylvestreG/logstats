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
#include <cstring>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#include <libfswatch/c++/event.hpp>
#include <libfswatch/c++/monitor_factory.hpp>
#include <utility>

#include "file_watcher.h"

// TODO
// - on file change update the fd...
clf::FileWatcher::FileWatcher(std::shared_ptr<clf::Config> config,
                              const std::filesystem::path &path)
    : _cfg{std::move(config)} {
  std::vector<std::string> paths;
  paths.emplace_back(path.string());

  // open the file
  _fd = open(path.c_str(), O_NONBLOCK | O_RDONLY | O_APPEND);
  if (_fd <= 0)
    throw std::runtime_error(strerror(errno));

  // go to the end of file
  if (lseek(_fd, 0, SEEK_END) == (off_t)-1)
    throw std::runtime_error(strerror(errno));

  _monitor = fsw::monitor_factory::create_monitor(
      fsw_monitor_type::system_default_monitor_type, paths,
      [](const std::vector<fsw::event> &events, void *arg) {
        auto self = static_cast<FileWatcher *>(arg);
        self->onWrite();
      },
      this);
  _monitor->add_event_type_filter({fsw_event_flag::Updated});
  _monitor->set_latency(0.01);
  _monitor->set_allow_overflow(true);
}

void clf::FileWatcher::start() {
  if (!_monitor->is_running())
    _fileWatchThread = std::thread([&]() { _monitor->start(); });
}

void clf::FileWatcher::stop() {
  if (_monitor->is_running()) {
    _monitor->stop();
    _fileWatchThread.join();
  }
}

void clf::FileWatcher::onWrite() {
  // we store data here to not lock at each read.
  std::vector<std::pair<Timepoint, std::string>> readVector;
  ssize_t size;

  do {
    std::string s;
    s.resize(_cfg->bufferSizeBytes());
    size = read(_fd, s.data(), _cfg->bufferSizeBytes());
    if (size > 0) {
      s.resize(size);

      readVector.emplace_back(std::chrono::system_clock::now(), std::move(s));
    }
  } while (size > 0);

  if (!readVector.empty())
    if (_buffCb)
      _buffCb(std::move(readVector));
}

clf::FileWatcher::~FileWatcher() {
  delete _monitor;
  close(_fd);
}

void clf::FileWatcher::setNewBufferCallback(newBufferCb cb) { _buffCb = cb; }
