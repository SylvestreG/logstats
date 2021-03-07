//
// Created by syl on 3/3/21.
//

#include <spdlog/spdlog.h>

#include "core.h"

Core::Core(std::shared_ptr<clf::Config> cfg, std::filesystem::path const &path)
    : _config{cfg}, _fileWatcher(cfg, path),
      _timer(_ioCtx, cfg->refreshTimeMs()), _ui(_data), _handleDataPool(_config->consumerThreadsNumber()) {
  _fileWatcher.setNewBufferCallback(
      [&](std::vector<std::pair<clf::Timepoint, std::string>> &&buffers) {
        this->onNewBuffer(std::move(buffers));
      });
  _splitter.setNewLineCallback([&](std::pair<Timepoint, std::string> &&line) {
    getDataFromSplitter(std::move(line));
  });
}

Core::~Core() {}

void Core::run() {
  _timer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });

  _splitter.start();
  _fileWatcher.start();

  _asioThread = std::thread([&]() { _ioCtx.run(); });

  _ui.render();

  spdlog::info("stopping ioctx");
  _ioCtx.stop();
  _asioThread.join();

  spdlog::info("stopping filewatcher");
  _fileWatcher.stop();

  spdlog::info("stopping splitter");
  _splitter.stop();


  _handleDataPool.join();
  spdlog::info("stopping data pool");

  spdlog::info("clfParser done");
}

void Core::onNewBuffer(
    std::vector<std::pair<clf::Timepoint, std::string>> &&buffers) {
  _splitter.pushIntoSplitter(std::move(buffers));
}

void Core::refreshDisplayCallback() {

  _timer.expires_at(_timer.expiry() + _config->refreshTimeMs());
  _timer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });
}

void Core::getDataFromSplitter(std::pair<Timepoint, std::string> &&line) {
  boost::asio::post(_handleDataPool, [this, line = std::move(line)]() {
    auto parsedData = parsers.parseLine(line.second);

    {
      std::lock_guard<std::mutex> lck(_data._dataMutex);
      _data._lastTenLines.push_front(line);

      _data._totalLines++;
      if (parsedData) {
        _data._lastTenSuccess.push_front(true);
        _data._totalValidLines++;
      } else {
        _data._lastTenSuccess.push_front(false);
      }

      if (_data._lastTenLines.size() > 10) {
        _data._lastTenLines.pop_back();
        _data._lastTenSuccess.pop_back();
      }
    }
  });
}
