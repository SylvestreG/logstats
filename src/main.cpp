#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

#include "config.h"
#include "core.h"

int main(int argc, char **argv) {
  std::filesystem::path confFile;
  std::filesystem::path inputFile;

  auto cli = lyra::cli() |
             lyra::opt(confFile, "conf")["-c"]["--conf"]("config file") |
             lyra::arg(inputFile, "inputFile")("log file to read");

  auto result = cli.parse({argc, argv});
  if (!result) {
    spdlog::error("error in command line {}", result.errorMessage());
    return EXIT_FAILURE;
  }

  if (inputFile.empty()) {
    spdlog::error("usage : ./clfMonitor logfile.log [-c config.json]");
    return EXIT_FAILURE;
  }

  if (!std::filesystem::exists(inputFile) ||
      !std::filesystem::is_regular_file(inputFile)) {
    spdlog::error("no such file {}", inputFile.string());
    return EXIT_FAILURE;
  }

  try {
    std::shared_ptr<clf::Config> cfg = std::make_shared<clf::Config>();
    if (confFile.empty())
      cfg = std::make_shared<clf::Config>();
    else
      cfg = std::make_shared<clf::Config>(confFile);

    Core c(cfg, inputFile);
    c.run();
  } catch (std::exception const &ex) {
    spdlog::error("leaving ... err={}", ex.what());
  }

  return EXIT_SUCCESS;
}
