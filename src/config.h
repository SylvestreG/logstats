#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <chrono>
#include <cstdint>
#include <filesystem>

using namespace std::chrono_literals;

namespace cfl {

class Config {
public:
  Config() = default;
  Config(std::filesystem::path file);
  ~Config() = default;
  Config(Config const &conf) = delete;
  Config &operator=(Config const &conf) = delete;

  [[nodiscard]] uint16_t batchMaxSizeNumber() const noexcept;
  [[nodiscard]] std::chrono::milliseconds batchMaxTime() const noexcept;
  [[nodiscard]] uint8_t consumerThreadsNumber() const noexcept;
  [[nodiscard]] bool debugEnabled() const noexcept;
  [[nodiscard]] std::chrono::milliseconds refreshTimeMs() const noexcept;
  [[nodiscard]] uint16_t alertThresholdNumber() const noexcept;

private:
  uint16_t _batchMaxSizeNumber{100};
  std::chrono::milliseconds _batchMaxTime{100ms};
  uint8_t _consumerThreadsNumber{4};
  bool _debugEnabled{true};
  std::chrono::milliseconds _refreshTimeMs{10s};
  uint16_t _alertThresholdNumber{10};
};

};     // namespace cfl

#endif /* __CONFIG_H__ */