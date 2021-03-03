//
// Created by syl on 3/3/21.
//

#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>

cfl::Config::Config(std::filesystem::path file) {
  std::ifstream ifs(file);

  try {
    nlohmann::json jf = nlohmann::json::parse(ifs);

    auto getParam = [&jf](char const *paramName, auto &valueToUpdate,
                          int minValue, int maxValue) {
      if (jf[paramName].is_number_integer()) {
        int intValue = jf[paramName];
        if (intValue < minValue) {
          spdlog::warn("ignore {} with negative value {}", paramName, intValue);
          return;
        }

        if (intValue > maxValue) {
          spdlog::warn("ignore {} with value too high {}", paramName, intValue);
          return;
        }

        if constexpr (std::is_same_v<std::chrono::milliseconds &,
                                     decltype(valueToUpdate)>) {
          valueToUpdate = std::chrono::milliseconds(intValue);
        } else {
          valueToUpdate = intValue;
        }
      }
    };

    getParam("batchMaxSizeNumber", _batchMaxSizeNumber, 0,
             std::numeric_limits<uint16_t>::max());
    getParam("batchMaxTimeMs", _batchMaxTime, 0,
             std::numeric_limits<uint16_t>::max());
    getParam("consumerThreadsNumber", _consumerThreadsNumber, 0,
             std::numeric_limits<uint8_t>::max());
    getParam("refreshTimeMs", _refreshTimeMs, 0,
             std::numeric_limits<uint16_t>::max());
    getParam("alertThresholdNumber", _alertThresholdNumber, 0,
             std::numeric_limits<uint16_t>::max());
    getParam("bufferSizeBytes", _bufferSizeBytes, 0,
             std::numeric_limits<uint16_t>::max());

    if (jf["debugEnabled"].is_boolean())
      _debugEnabled = jf["debugEnabled"];

  } catch (std::exception const &ex) {
    if (!std::filesystem::exists(file) ||
        !std::filesystem::is_regular_file(file))
      spdlog::error("config file [{}]: No such file ", file.string());
    else
      spdlog::error("parse pb: {} ", ex.what());
    throw ex;
  }
}

uint16_t cfl::Config::batchMaxSizeNumber() const noexcept {
  return _batchMaxSizeNumber;
}

std::chrono::milliseconds cfl::Config::batchMaxTime() const noexcept {
  return _batchMaxTime;
}

uint8_t cfl::Config::consumerThreadsNumber() const noexcept {
  return _consumerThreadsNumber;
}

bool cfl::Config::debugEnabled() const noexcept { return _debugEnabled; }

std::chrono::milliseconds cfl::Config::refreshTimeMs() const noexcept {
  return _refreshTimeMs;
}

uint16_t cfl::Config::alertThresholdNumber() const noexcept {
  return _alertThresholdNumber;
}

uint16_t cfl::Config::bufferSizeBytes() const noexcept {
  return _bufferSizeBytes;
}