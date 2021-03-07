//
// Created by sylvestre on 06/03/2021.
//
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#include <libfswatch/c++/event.hpp>
#include <libfswatch/c++/monitor_factory.hpp>
#include <utility>

#include "file_watcher.h"

FileWatcher::FileWatcher(std::shared_ptr<cfl::Config> config,
                         newBufferCb newBuffCb, const std::filesystem::path& path)
    : _cfg{std::move(config)}, _buffCb(std::move(newBuffCb)) {
  std::vector<std::string> paths;
  paths.emplace_back(path.string());

  _fd = open(path.c_str(), O_NONBLOCK | O_RDONLY);
  if (_fd <= 0)
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

void FileWatcher::start() {
  if (!_monitor->is_running())
    _fileWatchThread = std::thread([&]() { _monitor->start(); });
}

void FileWatcher::stop() {
  if (_monitor->is_running()) {
    _monitor->stop();
    _fileWatchThread.join();
  }
}

void FileWatcher::onWrite() {
  // we store data here to not lock at each read.
  std::vector<std::pair<Timepoint, std::string>> readVector;
  ssize_t size;

  do {
    std::string s;
    s.resize(_cfg->bufferSizeBytes());
    size = read(_fd, s.data(), _cfg->bufferSizeBytes());
    if (size > 0) {
      s.resize(size);
      // TODO lookup for existing buffer with same time
      readVector.emplace_back(std::chrono::system_clock::now(), std::move(s));
    }
  } while (size > 0);

  if (!readVector.empty())
    _buffCb(std::move(readVector));
}

FileWatcher::~FileWatcher() {
  stop();
  delete _monitor;
  close(_fd);
}
