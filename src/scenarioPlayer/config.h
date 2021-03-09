//
// Created by syl on 3/9/21.
//

#ifndef SCENARIO_CONFIG_H
#define SCENARIO_CONFIG_H

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
#endif // SCENARIO_CONFIG_H
