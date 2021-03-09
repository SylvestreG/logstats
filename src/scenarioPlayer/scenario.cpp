#include <filesystem>
#include <fstream>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

#include "config.h"

int main(int argc, char **argv) {
  std::filesystem::path confFile;
  std::filesystem::path inputFile;
  std::filesystem::path outputFile;
  try {
    auto cli = lyra::cli() |
               lyra::opt(confFile, "conf")["-c"]["--conf"]("config file") |
               lyra::opt(confFile, "input")["-i"]["--input"]("input file") |
               lyra::opt(confFile, "output")["-o"]["--output"]("output file");

    auto result = cli.parse({argc, argv});

    if (!result) {
      spdlog::error("error in command line {}", result.errorMessage());
      return EXIT_FAILURE;
    }

    if (outputFile.empty() || inputFile.empty()) {
      spdlog::error("usage : ./clfMonitor logfile.log [-c config.json]");
      return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(outputFile) ||
        !std::filesystem::is_regular_file(outputFile)) {
      spdlog::error("no such file {}", outputFile.string());
      return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(inputFile) ||
        !std::filesystem::is_regular_file(inputFile)) {
      spdlog::error("no such file {}", inputFile.string());
      return EXIT_FAILURE;
    }

    std::ofstream out;
    std::ifstream in;

    out.open(outputFile, std::ios::app);
    in.open(inputFile);

    if (!out.is_open()) {
      throw std::runtime_error(
          fmt::format("cannot open {}", outputFile.string()));
    }

    if (!in.is_open()) {
      throw std::runtime_error(
          fmt::format("cannot open {}", outputFile.string()));
    }

    std::string s;
    while (std::getline(in, s, '=')) {
      out << s;
    }

  } catch (std::exception const &ex) {
    spdlog::error("leaving ... err={}", ex.what());
  }
}
