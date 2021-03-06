#ifndef __FILE_WATCHER_H__
#define __FILE_WATCHER_H__

#include <atomic>
#include <filesystem>
#include <thread>

#include <libfswatch/c++/monitor.hpp>

#include "config.h"

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;
using newBufferCb = std::function<void (std::vector<std::pair<Timepoint, std::string>> &&)>;

class FileWatcher {
public:
  FileWatcher(std::shared_ptr<cfl::Config>, newBufferCb newBuffCb, std::filesystem::path);
  ~FileWatcher();

  FileWatcher() = delete;
  FileWatcher(FileWatcher const&) = delete;

  FileWatcher& operator=(FileWatcher const&) = delete;

  void start();
  void stop();

private:
  void onWrite();

  std::shared_ptr<cfl::Config> _cfg;

  fsw::monitor *_monitor;
  std::thread _fileWatchThread;

  newBufferCb _buffCb;
  int _fd;
};

#endif /* __FILE_WATCHER_H__ */