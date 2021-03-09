//
// Created by syl on 3/9/21.
//

#include "data.h"

void clf::MonitoringData::newLine(std::optional<ClfLine> &newObj,
                                  Timepoint time) {
  _totalLines++;

  if (newObj) {
    _totalValidLines++;
  }
}

void clf::GlobalMonitoringData::newLine(
    std::optional<ClfLine> &newObj,
    std::pair<clf::Timepoint, std::string> &&line) {
  MonitoringData::newLine(newObj, line.first);

  _lastTenLines.push_front(line);

  // removing timestamps
  _alertTs.push_back(line.first);
  std::remove_if(_alertTs.begin(), _alertTs.end(), [&](Timepoint tp) {
    if (tp < (line.first - _cfg->alertTimeMs()))
      return true;
    return false;
  });

  if (_alertTs.size() > _cfg->alertThresholdNumber())
    _alertOn = true;
  else
    _alertOn = false;

  if (newObj) {
    if (newObj->verb) {
      auto it = _verbMap.find(*newObj->verb);
      if (it == _verbMap.end())
        _verbMap.emplace(*newObj->verb, 1);
      else
        it->second++;
    }

    if (newObj->version)
      auto it = _versionArray[static_cast<int>(*newObj->version)]++;

    if (newObj->statusCode) {
      auto it = _statusMap.find(*newObj->statusCode);
      if (it == _statusMap.end())
        _statusMap.emplace(*newObj->statusCode, 1);
      else
        it->second++;
    }

    _lastTenSuccess.push_front(true);
  } else
    _lastTenSuccess.push_front(false);

  if (_lastTenLines.size() > 10) {
    _lastTenLines.pop_back();
    _lastTenSuccess.pop_back();
  }
}
clf::MonitoringData::MonitoringData(std::shared_ptr<clf::Config> cfg)
    : _cfg(cfg), _totalLines{0}, _totalValidLines{0},
      _startTime(std::chrono::system_clock::now()) {}

uint64_t clf::MonitoringData::totalLines() const { return _totalLines; }

uint64_t clf::MonitoringData::totalValidLines() const {
  return _totalValidLines;
}

clf::Timepoint clf::MonitoringData::startTime() const { return _startTime; }

clf::MonitoringData &
clf::MonitoringData::operator=(const clf::MonitoringData &data) {
  if (this != &data) {
    this->_startTime = data._startTime;
    this->_totalValidLines = data._totalValidLines;
    this->_totalLines = data._totalLines;
  }
  return *this;
}

clf::GlobalMonitoringData::GlobalMonitoringData(
    std::shared_ptr<clf::Config> cfg)
    : MonitoringData(cfg), _versionArray{0, 0, 0} {}

deque<std::pair<clf::Timepoint, std::string>> &
clf::GlobalMonitoringData::lastTenLines() {
  return _lastTenLines;
}

std::deque<bool> &clf::GlobalMonitoringData::lastTenSuccess() {
  return _lastTenSuccess;
}

std::unordered_map<boost::beast::http::verb, uint64_t> &
clf::GlobalMonitoringData::verbMap() {
  return _verbMap;
}

std::unordered_map<boost::beast::http::status, uint64_t> &
clf::GlobalMonitoringData::statusMap() {
  return _statusMap;
}

std::unordered_map<std::string, uint64_t> &
clf::GlobalMonitoringData::pathHitMap() {
  return _pathMapHit;
}

std::array<uint64_t, 3> &clf::GlobalMonitoringData::versionArray() {
  return _versionArray;
}

bool clf::GlobalMonitoringData::alertOn() { return _alertOn; }

vector<clf::Timepoint> &clf::GlobalMonitoringData::alertTs() {
  return _alertTs;
}

void clf::GlobalMonitoringData::setAlertOff() { _alertOn = false; }

clf::Data::Data(std::shared_ptr<clf::Config> cfg)
    : _globalData(cfg), _currentFrameData(cfg), _lastFrameFrameData(cfg) {}
