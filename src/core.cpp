//
// Created by syl on 3/3/21.
//
#include <cstdio>
#include <exception>
#include <spdlog/spdlog.h>

#include "core.h"

namespace {
auto registerEv = [](event *ev) {
  if (!ev)
    throw std::runtime_error("cannot create event");

  if (event_add(ev, nullptr) < 0)
    throw std::runtime_error("cannot add event");
};
};

Core::Core(std::shared_ptr<cfl::Config> cfg, std::filesystem::path const &path)
    : _config(cfg), _evBase(event_base_new()) {
  _evSignal = evsignal_new(
      _evBase, SIGINT,
      [](evutil_socket_t, short, void *arg) {
        auto self = static_cast<Core *>(arg);
        self->sigInt();
      },
      (void *)this);

  // open file and fseek to the end;
  _fd = fopen(path.string().c_str(), "r");
  if (!_fd)
    throw std::runtime_error("cannot open log file");

  fseek(_fd, 0, SEEK_END);

  _evRead = event_new(
      _evBase, fileno(_fd), EV_READ,
      [](evutil_socket_t, short, void *arg) {
        auto self = static_cast<Core *>(arg);
        self->onWrite();
      },
      (void *)this);

  registerEv(_evSignal);
  registerEv(_evRead);
}

Core::~Core() {
  event_free(_evSignal);
  event_free(_evRead);
  event_base_free(_evBase);
}

void Core::sigInt() {
  timeval tv{0, 100};
  spdlog::info("quit app");

  event_base_loopexit(_evBase, &tv);
}

void Core::run() { event_base_dispatch(_evBase); }

void Core::onWrite() {
  spdlog::info("on write");
  std::vector<uint8_t> buffer;
  buffer.resize(4096);
  auto size = fread(buffer.data(), 1, 4096, _fd);
  fseek(_fd, 0, SEEK_END);

  spdlog::info("{} bits", size);

  registerEv(_evRead);
}
