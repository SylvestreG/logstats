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
#include "config.h"
#include <fstream>
#include <gtest/gtest.h>

TEST(config, defaultVal) {
  clf::Config cfg;

  ASSERT_EQ(cfg.alertThresholdNumber(), 10);
  ASSERT_EQ(cfg.batchMaxSizeNumber(), 100);
  ASSERT_EQ(cfg.alertTimeMs(), 100ms);
  ASSERT_EQ(cfg.debugEnabled(), true);
  ASSERT_EQ(cfg.refreshTimeMs(), 10s);
  ASSERT_EQ(cfg.consumerThreadsNumber(), 4);
  ASSERT_EQ(cfg.bufferSizeBytes(), 4096);
}

TEST(config, fileDoesNotExist) {
  ASSERT_THROW(clf::Config("nonExistingFile.json"), std::exception);
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

  auto cfg = clf::Config(path);

  std::filesystem::remove(path);

  ASSERT_EQ(cfg.alertThresholdNumber(), 1);
  ASSERT_EQ(cfg.batchMaxSizeNumber(), 1);
  ASSERT_EQ(cfg.debugEnabled(), false);
  ASSERT_EQ(cfg.consumerThreadsNumber(), 1);
  ASSERT_EQ(cfg.bufferSizeBytes(), 1);
  ASSERT_EQ(cfg.refreshTimeMs().count(), (1ms).count());
  ASSERT_EQ(cfg.alertTimeMs().count(), (1ms).count());
}