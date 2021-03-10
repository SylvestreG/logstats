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
#include "config.h"
#include <fstream>
#include <generator/config.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>

clf::Config::Config(const std::filesystem::path &file) {
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

    getParam("alertTimeMs", _alertTimeMs, 500,
             std::numeric_limits<uint16_t>::max());
    getParam("consumerThreadsNumber", _consumerThreadsNumber, 1,
             std::numeric_limits<uint8_t>::max());
    getParam("refreshTimeMs", _refreshTimeMs, 1000,
             std::numeric_limits<uint16_t>::max());
    getParam("alertThresholdNumber", _alertThresholdNumber, 1,
             std::numeric_limits<uint16_t>::max());
    getParam("bufferSizeBytes", _bufferSizeBytes, 100,
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

std::chrono::milliseconds clf::Config::alertTimeMs() const noexcept {
  return _alertTimeMs;
}

uint8_t clf::Config::consumerThreadsNumber() const noexcept {
  return _consumerThreadsNumber;
}

bool clf::Config::debugEnabled() const noexcept { return _debugEnabled; }

std::chrono::milliseconds clf::Config::refreshTimeMs() const noexcept {
  return _refreshTimeMs;
}

uint16_t clf::Config::alertThresholdNumber() const noexcept {
  return _alertThresholdNumber;
}

uint16_t clf::Config::bufferSizeBytes() const noexcept {
  return _bufferSizeBytes;
}

std::string clf::Config::dumpToJson() const noexcept {
  nlohmann::json jf;
  jf["alertTimeMs"] = _alertTimeMs.count();
  jf["consumerThreadsNumber"] = _consumerThreadsNumber;
  jf["debugEnabled"] = _debugEnabled;
  jf["refreshTimeMs"] = _refreshTimeMs.count();
  jf["alertThresholdNumber"] = _alertThresholdNumber;
  jf["bufferSizeBytes"] = _bufferSizeBytes;

  return jf.dump();
}
