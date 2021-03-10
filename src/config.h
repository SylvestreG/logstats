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
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <chrono>
#include <cstdint>
#include <filesystem>

using namespace std::chrono_literals;

namespace clf {

class Config {
public:
  Config() = default;
  explicit Config(const std::filesystem::path &file);
  ~Config() = default;
  Config(Config const &conf) = delete;
  Config &operator=(Config const &conf) = delete;

  [[nodiscard]] std::chrono::milliseconds alertTimeMs() const noexcept;
  [[nodiscard]] uint8_t consumerThreadsNumber() const noexcept;
  [[nodiscard]] bool debugEnabled() const noexcept;
  [[nodiscard]] std::chrono::milliseconds refreshTimeMs() const noexcept;
  [[nodiscard]] uint16_t alertThresholdNumber() const noexcept;
  [[nodiscard]] uint16_t bufferSizeBytes() const noexcept;

  std::string dumpToJson() const noexcept;

private:
  std::chrono::milliseconds _alertTimeMs{2s};
  uint8_t _consumerThreadsNumber{10};
  bool _debugEnabled{false};
  std::chrono::milliseconds _refreshTimeMs{10s};
  uint16_t _alertThresholdNumber{10};
  uint16_t _bufferSizeBytes{4096};
};

} // namespace clf

#endif // __CONFIG_H__