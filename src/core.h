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
#include "ui.h"

class Core {
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

  // Timers
  boost::asio::io_context _ioCtx;
  boost::asio::steady_timer _timer;

  // Buffer mgmt
  clf::BufferSplitter _splitter;
  clf::FileWatcher _fileWatcher;

  // Config
  std::shared_ptr<clf::Config> _config;

  // Parser
  std::vector<clf::Parser> parsers;

  // Ui
  clf::Ui _ui;
};

#endif /* __CORE_H__ */