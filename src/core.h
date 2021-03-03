#ifndef __CORE_H__
#define __CORE_H__

#include "config.h"
#include <boost/asio.hpp>
#include <thread>

class Core {
public:
  explicit Core(cfl::Config const &cfg);
  Core() = delete;
  ~Core() = default;
  Core(Core const &) = delete;
  Core &operator=(Core const &) = delete;

private:
  cfl::Config const &_config;
  std::thread _producers;
  boost::asio::thread_pool _consumers;
};

#endif /* __CORE_H__ */