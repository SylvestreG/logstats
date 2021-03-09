//
// Created by syl on 3/3/21.
//

#include <spdlog/spdlog.h>

#include "core.h"

Core::Core(std::shared_ptr<clf::Config> cfg, std::filesystem::path const &path)
    : _config{cfg}, _fileWatcher(cfg, path),
      _refreshTimer(_ioCtx, cfg->refreshTimeMs()), _alertOffTimer(_ioCtx),
      _data(cfg), _ui(_data),
      _handleDataPool(_config->consumerThreadsNumber()) {
  _fileWatcher.setNewBufferCallback(
      [&](std::vector<std::pair<clf::Timepoint, std::string>> &&buffers) {
        this->onNewBuffer(std::move(buffers));
      });
  _splitter.setNewLineCallback([&](std::pair<Timepoint, std::string> &&line) {
    getDataFromSplitter(std::move(line));
  });
}

Core::~Core() = default;

void Core::run() {
  _refreshTimer.async_wait(
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
  auto ts = std::chrono::system_clock::now();
  _data._lastFrameFrameData = _data._currentFrameData;
  _data._currentFrameData = clf::MonitoringData(_config);
  _data._currentFrameData.startTime() = ts;

  _refreshTimer.expires_at(_refreshTimer.expiry() + _config->refreshTimeMs());
  _refreshTimer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });
}

void Core::getDataFromSplitter(std::pair<Timepoint, std::string> &&line) {
  boost::asio::post(_handleDataPool, [this, line = std::move(line)]() {
    auto parsedData = parsers.parseLine(line.second);

    {
      std::lock_guard<std::mutex> lck(_data._dataMutex);

      _data._globalData.newLine(parsedData, {line.first, line.second});
      _data._currentFrameData.newLine(parsedData, line.first);

      // alert need to update timer to starving point
      // need to take the ts of nbThreshold -1 and add time.
      if (_data._globalData.alertOn()) {
        _alertOffTimer.cancel();

        auto alertsTs = _data._globalData.alertTs();
        auto alertOffTs =
            alertsTs[alertsTs.size() - (_config->alertThresholdNumber() - 1)] +
            _config->alertTimeMs();

        _alertOffTimer.expires_after(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                alertOffTs - std::chrono::system_clock::now()));
        _alertOffTimer.async_wait([&](boost::system::error_code const &) {
          std::lock_guard<std::mutex> lck(_data._dataMutex);
          _data._globalData.setAlertOff();
        });
      }
    }
  });
}
