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
#include "data.h"
#include "file_watcher.h"
#include "parser.h"
#include "ui.h"

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
  boost::asio::steady_timer _refreshTimer;
  boost::asio::steady_timer _alertOffTimer;
  std::thread _asioThread;

  // Buffer mgmt
  clf::BufferSplitter _splitter;
  clf::FileWatcher _fileWatcher;

  // Config
  std::shared_ptr<clf::Config> _config;

  // data
  clf::Data _data;

  // Ui
  clf::Ui _ui;

  // Parser
  clf::Parser parsers;

  //handle data
  boost::asio::thread_pool _handleDataPool;
};

#endif /* __CORE_H__ */