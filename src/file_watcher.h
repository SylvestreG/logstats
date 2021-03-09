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

#endif /* __FILE_WATCHER_H__ */