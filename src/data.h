//
// Created by syl on 3/7/21.
//

#ifndef LOGSTATS_DATA_H
#define LOGSTATS_DATA_H

#include <chrono>
#include <deque>
#include <mutex>

namespace clf {

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

struct Data {
  std::mutex _dataMutex;
  std::deque<std::pair<Timepoint, std::string>> _lastTenLines;
  std::deque<bool> _lastTenSuccess;

  uint64_t _totalLines{0};
  uint64_t _totalValidLines{0};

  Timepoint _appStartTime{std::chrono::system_clock::now()};

};
} // namespace clf

#endif // LOGSTATS_DATA_H
