#ifndef __CORE_H__
#define __CORE_H__

#include <filesystem>
#include <thread>
#include <event2/event.h>

#include "config.h"

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
  //libevent backend
  event_base *_evBase;
  event *_evRead;
  event *_evSignal;

  //file that we monitor
  FILE *_fd;

  //data container
  std::vector<std::string> buf;

  std::shared_ptr<cfl::Config> _config;
  std::thread _producers;
};

#endif /* __CORE_H__ */