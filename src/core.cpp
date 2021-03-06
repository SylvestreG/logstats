//
// Created by syl on 3/3/21.
//

#include "core.h"
#include <spdlog/spdlog.h>

Core::Core(std::shared_ptr<cfl::Config> cfg, std::filesystem::path const &path)
    : _config{cfg},
      _fileWatcher{
          cfg,
          [&](std::vector<std::pair<Timepoint, std::string>> &&buffers) {
            this->onNewBuffer(std::move(buffers));
          },
          path},
      _timer(_ioCtx, cfg->refreshTimeMs()) {
  _timer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });
}

Core::~Core() {
  _fileWatcher.stop();
  _splitter.stop();
}

void Core::run() {
  _splitter.start();
  _fileWatcher.start();

  spdlog::info("launching asio");
  _ioCtx.run();

  _fileWatcher.stop();
  _splitter.stop();
  spdlog::info("asio done");
}

void Core::sigInt() {}

void Core::onNewBuffer(
    std::vector<std::pair<Timepoint, std::string>> &&buffers) {
  _splitter.pushIntoSplitter(std::move(buffers));
}

void Core::refreshDisplayCallback() {
  spdlog::info("refresh");
  _timer.expires_at(_timer.expiry() + _config->refreshTimeMs());
  _timer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });
}
