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