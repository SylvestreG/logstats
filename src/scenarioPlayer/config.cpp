//
// Created by syl on 3/9/21.
//

#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "config.h"

Config::Config(const std::filesystem::path &file) {
  std::ifstream ifs(file);

  try {
    nlohmann::json jf = nlohmann::json::parse(ifs);
    if (jf["scenarios"].is_array()) {
      _scenarios.clear();
      for (auto it = jf["scenarios"].begin(); it != jf["scenarios"].end();
           ++it) {
        if (it->is_array() && it->size() == 2) {
          if ((*it)[0].is_number_integer() && (*it)[1].is_number_integer()) {
            _scenarios.emplace_back((*it)[0],
                                    std::chrono::milliseconds{(*it)[1]});
          }
        }
      }
    }
  } catch (std::exception const &ex) {
    if (!std::filesystem::exists(file) ||
        !std::filesystem::is_regular_file(file))
      spdlog::error("config file [{}]: No such file ", file.string());
    else
      spdlog::error("parse pb: {} ", ex.what());
    throw ex;
  }
}

std::vector<std::pair<uint32_t, std::chrono::milliseconds>>
Config::scenarios() const noexcept {
  return _scenarios;
}