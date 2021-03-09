//
// Created by syl on 3/3/21.
//

#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>

Config::Config(const std::filesystem::path &file) {
  std::ifstream ifs(file);

  try {
    nlohmann::json jf = nlohmann::json::parse(ifs);

    auto getParam = [&jf](char const *paramName, auto &valueToUpdate,
                          int minValue, int maxValue) {
      if (jf[paramName].is_number_integer()) {
        int intValue = jf[paramName];
        if (intValue < minValue) {
          spdlog::warn("ignoring {} with negative value {}", paramName,
                       intValue);
          return;
        }

        if (intValue > maxValue) {
          spdlog::warn("ignoring {} with value too high {}", paramName,
                       intValue);
          return;
        }

        if constexpr (std::is_same_v<std::chrono::milliseconds &,
                                     decltype(valueToUpdate)>)
          valueToUpdate = std::chrono::milliseconds(intValue);
        else if constexpr (std::is_same_v<std::string &,
                                          decltype(valueToUpdate)> ||
                           std::is_same_v<boost::beast::http::verb &,
                                          decltype(valueToUpdate)> ||
                           std::is_same_v<std::filesystem::path &,
                                          decltype(valueToUpdate)>)
          spdlog::warn("ignoring field for {} need string", paramName);
        else if constexpr (std::is_same_v<boost::beast::http::status &,
                                          decltype(valueToUpdate)>)
          valueToUpdate = boost::beast::http::int_to_status(intValue);
        else
          valueToUpdate = intValue;
      } else if (jf[paramName].is_string()) {
        if constexpr (std::is_same_v<std::string &, decltype(valueToUpdate)>)
          valueToUpdate = jf[paramName];
        else if constexpr (std::is_same_v<std::filesystem::path &,
                                          decltype(valueToUpdate)>)
          valueToUpdate = std::filesystem::path(jf[paramName]);
        else if constexpr (std::is_same_v<boost::beast::http::verb &,
                                          decltype(valueToUpdate)>) {
          valueToUpdate = boost::beast::http::string_to_verb(jf[paramName]);
        }
      }
    };

    getParam("requestNumber", _requestNumber, 0,
             std::numeric_limits<uint16_t>::max());
    getParam("delayBetweenRequestMs", _delayBetweenRequestMs, 0,
             std::numeric_limits<uint16_t>::max());
    getParam("ipAddr", _ip, 0, 0);
    getParam("userIdentifier", _userIdentifer, 0, 0);
    getParam("userId", _userId, 0, 0);
    getParam("verb", _httpVerb, 0, 0);
    getParam("path", _path, 0, 0);
    getParam("httpVersion", _httpVersion, 0, 0);
    getParam("httpStatus", _status, 0, std::numeric_limits<uint16_t>::max());
    getParam("size", _size, 0, std::numeric_limits<uint16_t>::max());

  } catch (std::exception const &ex) {
    if (!std::filesystem::exists(file) ||
        !std::filesystem::is_regular_file(file))
      spdlog::error("config file [{}]: No such file ", file.string());
    else
      spdlog::error("parse pb: {} ", ex.what());
    throw ex;
  }

  if (_httpVersion != "HTTP/1.0" && _httpVersion != "HTTP/1.1")
    _httpVersion = "HTTP/1.1";
}

uint16_t Config::requestNumber() const noexcept { return _requestNumber; }

std::chrono::milliseconds Config::delayBetweenRequestMs() const noexcept {
  return _delayBetweenRequestMs;
}

std::string Config::ipAddr() const noexcept { return _ip; }

std::string Config::userIdentifier() const noexcept { return _userIdentifer; }

std::string Config::userId() const noexcept { return _userId; }

boost::beast::http::verb Config::httpVerb() const noexcept { return _httpVerb; }

std::filesystem::path Config::path() const noexcept { return _path; }

std::string Config::httpVersion() const noexcept { return _httpVersion; }

boost::beast::http::status Config::httpStatus() const noexcept {
  return _status;
}

uint32_t Config::size() const noexcept { return _size; }