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
#ifndef __SCENARIO_CONFIG_H__
#define __SCENARIO_CONFIG_H__

#include <chrono>
#include <filesystem>
#include <vector>

using namespace std::chrono_literals;

class Config {
public:
  Config() = default;
  explicit Config(const std::filesystem::path &file);
  ~Config() = default;
  Config(Config const &conf) = delete;
  Config &operator=(Config const &conf) = delete;

  // requestStuff
  [[nodiscard]] std::vector<std::pair<uint32_t, std::chrono::milliseconds>>
  scenarios() const noexcept;

private:
  std::vector<std::pair<uint32_t, std::chrono::milliseconds>> _scenarios{
      {12, 1000ms}, {20, 100ms}, {10, 5ms}, {5, 2000ms}, {100, 500ms}};
};
#endif // SCENARIO_CON__SCENARIO_CONFIG_H__FIG_H
