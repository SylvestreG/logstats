//
// Created by syl on 3/7/21.
//

#ifndef LOGSTATS_DATA_H
#define LOGSTATS_DATA_H

#include <chrono>
#include <deque>
#include <mutex>
#include <set>

#include "config.h"
#include "parser.h"

namespace clf {

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

class MonitoringData {
public:
  MonitoringData(std::shared_ptr<clf::Config> cfg);
  ~MonitoringData() = default;
  MonitoringData &operator=(MonitoringData const &);
  MonitoringData(MonitoringData const &) = delete;

  void newLine(std::optional<ClfLine> &newObj, Timepoint time);

  uint64_t totalLines() const;
  uint64_t totalValidLines() const;
  Timepoint startTime() const;

protected:
  std::shared_ptr<clf::Config> _cfg;
  uint64_t _totalLines{0};
  uint64_t _totalValidLines{0};

  Timepoint _startTime{std::chrono::system_clock::now()};
};

class GlobalMonitoringData : public MonitoringData {
public:
  GlobalMonitoringData(std::shared_ptr<clf::Config> cfg);
  ~GlobalMonitoringData() = default;
  GlobalMonitoringData &operator=(MonitoringData const &) = delete;
  GlobalMonitoringData(MonitoringData const &) = delete;

  void newLine(std::optional<ClfLine> &newObj,
               std::pair<clf::Timepoint, std::string> &&line);

  std::deque<std::pair<Timepoint, std::string>> &lastTenLines();
  std::deque<bool> &lastTenSuccess();
  std::unordered_map<boost::beast::http::verb, uint64_t> &verbMap();
  std::unordered_map<boost::beast::http::status, uint64_t> &statusMap();
  std::unordered_map<std::string, uint64_t> &pathHitMap();
  std::array<uint64_t, 3> &versionArray();
  std::vector<Timepoint>& alertTs();
  bool alertOn();
  void setAlertOff();

private:
  std::deque<std::pair<Timepoint, std::string>> _lastTenLines;
  std::deque<bool> _lastTenSuccess;
  std::unordered_map<boost::beast::http::verb, uint64_t> _verbMap;
  std::unordered_map<boost::beast::http::status, uint64_t> _statusMap;
  std::unordered_map<std::string, uint64_t> _pathMapHit;
  std::array<uint64_t, 3> _versionArray;

  std::vector<Timepoint> _alertTs;
  bool _alertOn;
};

struct Data {
  Data(std::shared_ptr<clf::Config> cfg);

  std::mutex _dataMutex;

  GlobalMonitoringData _globalData;
  MonitoringData _currentFrameData;
  MonitoringData _lastFrameFrameData;

};
} // namespace clf

#endif // LOGSTATS_DATA_H
