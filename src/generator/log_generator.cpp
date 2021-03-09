#include <fstream>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

#include "config.h"

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

  std::ofstream out;
  out.open(inputFile, std::ios::app);

  try {
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
      out << fmt::format(output, buffer) << std::endl;

      std::this_thread::sleep_for(cfg->delayBetweenRequestMs());
    }
  } catch (std::exception const &ex) {
    spdlog::error("leaving ... err={}", ex.what());
  }
}