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
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

#include "config.h"

int main(int argc, char **argv) {
  std::filesystem::path confFile;
  std::filesystem::path outputFile;

  try {
    auto cli = lyra::cli() |
               lyra::opt(confFile, "conf")["-c"]["--conf"]("config file") |
               lyra::arg(outputFile, "inputFile")("log file to read");

    auto result = cli.parse({argc, argv});
    if (!result) {
      spdlog::error("error in command line {}", result.errorMessage());
      return EXIT_FAILURE;
    }

    if (outputFile.empty()) {
      spdlog::error("usage : ./clfMonitor logfile.log [-c config.json]");
      return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(outputFile) ||
        !std::filesystem::is_regular_file(outputFile)) {
      spdlog::error("no such file {}", outputFile.string());
      return EXIT_FAILURE;
    }

    int out;

    std::shared_ptr<Config> cfg = std::make_shared<Config>();
    if (confFile.empty())
      cfg = std::make_shared<Config>();
    else
      cfg = std::make_shared<Config>(confFile);

    std::string output;
    output =
        fmt::format("{} {} {} [{{}}] \"{} {} {}\" {} {}", cfg->ipAddr(),
                    cfg->userIdentifier(), cfg->userId(),
                    boost::beast::http::to_string(cfg->httpVerb()).to_string(),
                    cfg->path().string(), cfg->httpVersion(),
                    static_cast<int>(cfg->httpStatus()), cfg->size());
    char buffer[80];

    for (int i = 0; i < cfg->requestNumber(); i++) {
      std::chrono::system_clock::time_point now =
          std::chrono::system_clock::now();
      std::time_t now_c = std::chrono::system_clock::to_time_t(now);
      std::tm now_tm = *std::localtime(&now_c);
      std::strftime(buffer, 80, "%d/%b/%Y:%H:%M:%S %z", &now_tm);
      std::string line = fmt::format(output, buffer);
      line.append("\n");
      out = open(outputFile.c_str(), O_APPEND | O_WRONLY);
      if (out <= 0) {
        throw std::runtime_error(
            fmt::format("cannot open {}", outputFile.string()));
      }

      write(out, line.c_str(), line.size());
      close(out);
      std::this_thread::sleep_for(cfg->delayBetweenRequestMs());
    }
  } catch (std::exception const &ex) {
    spdlog::error("leaving ... err={}", ex.what());
  }
}