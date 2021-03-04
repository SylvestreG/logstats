//
// Created by syl on 3/3/21.
//
#include <cstdio>
#include <signal.h>

#include <libfswatch/c++/event.hpp>
#include <libfswatch/c++/monitor_factory.hpp>
#include <spdlog/spdlog.h>

#include "core.h"

Core::Core(std::shared_ptr<cfl::Config> cfg, std::filesystem::path const &path)
    : _config(cfg), _ifs(path, std::ios_base::ate) {

  std::vector<std::string> paths;
  paths.emplace_back(path.string());

  if (!_ifs.is_open())
    throw std::runtime_error("cannot open log file");

  _monitor = fsw::monitor_factory::create_monitor(
      fsw_monitor_type::system_default_monitor_type, paths,
      [](const std::vector<fsw::event> &events, void *arg) {
        auto self = static_cast<Core *>(arg);
        self->onWrite();
      },
      this);
  _monitor->add_event_type_filter({fsw_event_flag::Updated});
  _monitor->set_latency(0.01);
  _monitor->set_allow_overflow(true);

  _monitorThread = std::thread([&]() { _monitor->start(); });
}

Core::~Core() {
  _monitor->stop();
  delete _monitor;
  _ifs.close();
}

void
Core::run() {
  _monitorThread.join();
}

void Core::sigInt() {}

void Core::onWrite() {
  size_t size{1};

  while (size != 0) {
    std::string buf;
    buf.reserve(_config->bufferSizeBytes());

    size = _ifs.readsome(buf.data(), _config->bufferSizeBytes());
    buf.resize(buf.size() + size);
    spdlog::info("read {} bits", buf.size());
  }
}
