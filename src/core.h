#ifndef __CORE_H__
#define __CORE_H__

#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <thread>

#include <boost/asio.hpp>

#include "buffer_splitter.h"
#include "config.h"
#include "file_watcher.h"
#include "parser.h"

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

class Core : public boost::enable_shared_from_this<Core> {
public:
  explicit Core(std::shared_ptr<clf::Config> cfg,
                std::filesystem::path const &path);
  Core() = delete;
  ~Core();
  Core(Core const &) = delete;
  Core &operator=(Core const &) = delete;

  void run();

protected:
  void
  onNewBuffer(std::vector<std::pair<clf::Timepoint, std::string>> &&buffers);

private:
  void refreshDisplayCallback();
  void getDataFromSplitter(std::pair<Timepoint, std::string> &&line);

  // Timers
  boost::asio::io_context _ioCtx;
  boost::asio::steady_timer _timer;
  std::thread _asioThread;

  // Buffer mgmt
  clf::BufferSplitter _splitter;
  clf::FileWatcher _fileWatcher;

  // Config
  std::shared_ptr<clf::Config> _config;

  // data
  std::mutex _dataMutex;
  std::deque<std::pair<Timepoint, std::string>> _lastTenLines;

  // Parser
  std::vector<clf::Parser> parsers;
};

#endif /* __CORE_H__ */