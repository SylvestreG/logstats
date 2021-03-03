#ifndef __CORE_H__
#define __CORE_H__

#include "config.h"
#include <boost/asio.hpp>
#include <thread>

class Core {
public:
  explicit Core(std::shared_ptr<cfl::Config> cfg);
  Core() = delete;
  ~Core() = default;
  Core(Core const &) = delete;
  Core &operator=(Core const &) = delete;

private:
  std::shared_ptr<cfl::Config> _config;
  std::thread _producers;
  boost::asio::thread_pool _consumers;
};

#endif /* __CORE_H__ */