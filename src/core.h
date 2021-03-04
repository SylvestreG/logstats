#ifndef __CORE_H__
#define __CORE_H__

#include <chrono>
#include <fstream>
#include <map>
#include <filesystem>
#include <thread>

#include <boost/asio.hpp>
#include <libfswatch/c++/monitor.hpp>

#include "config.h"

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

class Core {
public:
  explicit Core(std::shared_ptr<cfl::Config> cfg, std::filesystem::path const& path);
  Core() = delete;
  ~Core();
  Core(Core const &) = delete;
  Core &operator=(Core const &) = delete;

  void run();

protected:
  void sigInt();
  void onWrite();

private:
  //fs events
  fsw::monitor *_monitor;
  std::thread _monitorThread;
  std::thread _lineSplitterThread;
  boost::asio::thread_pool _workerThreads;

  //file that we monitor
  std::ifstream _ifs;

  //data container
  std::map<Timepoint, std::string> bufferQueue;

  std::shared_ptr<cfl::Config> _config;
  std::thread _producers;
};

#endif /* __CORE_H__ */