#include <spdlog/spdlog.h>
#include "config.h"
#include "core.h"

int main([[maybe_unused]]int ac, [[maybe_unused]]char **av) {
  try {
    cfl::Config cfg(av[1]);
    Core c(cfg);
  } catch (...) {
    spdlog::error("leaving ...");
  }

}