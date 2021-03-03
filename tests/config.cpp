#include "config.h"
#include <fstream>
#include <gtest/gtest.h>

TEST(config, defaultVal) {
  cfl::Config cfg;

  ASSERT_EQ(cfg.alertThresholdNumber(), 10);
  ASSERT_EQ(cfg.batchMaxSizeNumber(), 100);
  ASSERT_EQ(cfg.batchMaxTime(), 100ms);
  ASSERT_EQ(cfg.debugEnabled(), true);
  ASSERT_EQ(cfg.refreshTimeMs(), 10s);
  ASSERT_EQ(cfg.consumerThreadsNumber(), 4);
  ASSERT_EQ(cfg.bufferSizeBytes(), 4096);
}

TEST(config, fileDoesNotExist) {
  ASSERT_THROW(cfl::Config("nonExistingFile.json"), std::exception);
}

TEST(config, goodConfig) {
  auto path = std::filesystem::temp_directory_path();
  path.append("newJson.json");
  std::ofstream ofs;
  ofs.open(path);
  ofs << R"foo({
  "batchMaxSizeNumber": 1,
  "batchMaxTimeMs": 1,
  "consumerThreadsNumber": 1,
  "debugEnabled": false,
  "refreshTimeMs": 1,
  "alertThresholdNumber" : 1,
  "bufferSizeBytes": 1
}
)foo";
  ofs.close();

  auto cfg = cfl::Config(path);

  std::filesystem::remove(path);

  ASSERT_EQ(cfg.alertThresholdNumber(), 1);
  ASSERT_EQ(cfg.batchMaxSizeNumber(), 1);
  ASSERT_EQ(cfg.debugEnabled(), false);
  ASSERT_EQ(cfg.consumerThreadsNumber(), 1);
  ASSERT_EQ(cfg.bufferSizeBytes(), 1);
  ASSERT_EQ(cfg.refreshTimeMs().count(), (1ms).count());
  ASSERT_EQ(cfg.batchMaxTime().count(), (1ms).count());
}