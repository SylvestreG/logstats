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
  void onNewBuffer(std::vector<std::pair<clf::Timepoint, std::string>> &&buffers);

private:
  void refreshDisplayCallback();

  // fs events
  boost::asio::io_context _ioCtx;
  boost::asio::steady_timer _timer;

  clf::BufferSplitter _splitter;
  clf::FileWatcher _fileWatcher;

  // data container

  std::shared_ptr<clf::Config> _config;
  std::vector<clf::Parser> parsers;
};

#endif /* __CORE_H__ */