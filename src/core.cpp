/*
 * Copyright (c) 2021 Sylvestre Gallon
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <spdlog/spdlog.h>

#include "core.h"

Core::Core(std::shared_ptr<clf::Config> cfg, std::filesystem::path const &path)
    : _config{cfg}, _fileWatcher(cfg, path),
      _refreshTimer(_ioCtx, cfg->refreshTimeMs()),
      _alertOffTimer(_ioCtx, std::chrono::milliseconds{50}), _data(cfg),
      _ui(_data), _handleDataPool(_config->consumerThreadsNumber()) {
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
  _alertOffTimer.async_wait(
      [&](boost::system::error_code const &) { checkAlarmCallback(); });

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
  {
    std::lock_guard<std::mutex> lck(_data._dataMutex);
    auto ts = std::chrono::system_clock::now();
    _data._lastFrameFrameData = _data._currentFrameData;
    _data._currentFrameData = clf::MonitoringData(_config);
    _data._currentFrameData.startTime() = ts;
  }

  _refreshTimer.expires_at(_refreshTimer.expiry() + _config->refreshTimeMs());
  _refreshTimer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });
}

void Core::checkAlarmCallback() {
  {
    std::lock_guard<std::mutex> lck(_data._dataMutex);
    _data._globalData.checkAlarm();
  }
  _alertOffTimer.expires_at(_alertOffTimer.expiry() +
                            std::chrono::milliseconds(50));
  _alertOffTimer.async_wait(
      [&](boost::system::error_code const &) { checkAlarmCallback(); });
}

void Core::getDataFromSplitter(std::pair<Timepoint, std::string> &&line) {
  boost::asio::post(_handleDataPool, [this, line = std::move(line)]() {
    clf::Parser parsers;
    auto parsedData = parsers.parseLine(line.second);
    {
      std::lock_guard<std::mutex> lck(_data._dataMutex);

      _data._globalData.newLine(parsedData, {line.first, line.second});
      _data._currentFrameData.newLine(parsedData, line.first);
    }
  });
}
