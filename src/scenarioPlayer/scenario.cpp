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
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

#include "config.h"

int main(int argc, char **argv) {
  std::filesystem::path confFile;
  std::filesystem::path inputFile;
  std::filesystem::path outputFile;
  try {
    auto cli = lyra::cli() |
               lyra::opt(confFile, "conf")["-c"]["--conf"]("config file") |
               lyra::opt(inputFile, "input")["-i"]["--input"]("input file") |
               lyra::opt(outputFile, "output")["-o"]["--output"]("output file");

    auto result = cli.parse({argc, argv});

    if (!result) {
      spdlog::error("error in command line {}", result.errorMessage());
      return EXIT_FAILURE;
    }

    if (outputFile.empty() || inputFile.empty()) {
      spdlog::error(
          "usage : ./clfMonitor -i input.log -o output.log [-c config.json]");
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

    int out;
    std::ifstream in;

    in.open(inputFile);

    if (!in.is_open()) {
      throw std::runtime_error(
          fmt::format("cannot open {}", inputFile.string()));
    }

    std::shared_ptr<Config> cfg = std::make_shared<Config>();
    if (confFile.empty())
      cfg = std::make_shared<Config>();
    else
      cfg = std::make_shared<Config>(confFile);

    std::string line;
    for (auto s : cfg->scenarios()) {
      for (int i = 0; i < s.first; i++) {
        if (!std::getline(in, line))
          return EXIT_SUCCESS;
        spdlog::info("found {}", line);
        line.append("\n");
        out = open(outputFile.c_str(), O_APPEND | O_WRONLY);
        if (out <= 0) {
          throw std::runtime_error(
              fmt::format("cannot open {}", outputFile.string()));
        }

        write(out, line.c_str(), line.size());
        close(out);
        std::this_thread::sleep_for(s.second);
      }
    }

  } catch (std::exception const &ex) {
    spdlog::error("leaving ... err={}", ex.what());
  }
}
