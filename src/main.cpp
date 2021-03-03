#include "config.h"
#include "core.h"
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
  std::string confFile;
  std::string inputFile;

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
  }

  try {
    std::shared_ptr<cfl::Config> cfg;
    if (confFile.empty())
      cfg = std::make_shared<cfl::Config>();
    else
      cfg = std::make_shared<cfl::Config>(confFile);

    Core c(cfg);
  } catch (...) {
    spdlog::error("leaving ...");
  }

  return EXIT_SUCCESS;
}