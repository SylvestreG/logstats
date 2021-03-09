//
// Created by syl on 3/9/21.
//

#include "data.h"

void clf::MonitoringData::newLine(std::optional<ClfLine> &newObj,
                                  Timepoint time) {
  _totalLines++;

  if (newObj) {
    _totalValidLines++;

    if (newObj->verb) {
      auto it = _verbMap.find(*newObj->verb);
      if (it == _verbMap.end())
        _verbMap.emplace(*newObj->verb, 1);
      else
        it->second++;
    }

    if (newObj->version)
      auto it = _versionArray[static_cast<int>(*newObj->version)]++;

    if (newObj->path) {
      auto it = _pathMap.find(*newObj->path);
      if (it == _pathMap.end())
        _pathMap.emplace(*newObj->path, 1);
      else
        it->second++;
    }

    if (newObj->statusCode) {
      auto it = _statusMap.find(*newObj->statusCode);
      if (it == _statusMap.end())
        _statusMap.emplace(*newObj->statusCode, 1);
      else
        it->second++;
    }

    if (newObj->objectSize)
      _totalSize += *newObj->objectSize;
  }
}

void clf::GlobalMonitoringData::newLine(
    std::optional<ClfLine> &newObj,
    std::pair<clf::Timepoint, std::string> &&line) {
  MonitoringData::newLine(newObj, line.first);

  _lastTenLines.push_front(line);

  // removing timestamps
  _alertTs.push_back(line.first);
  checkAlarm();
  if (_alertOn)
    _hits++;

  if (newObj) {
    _lastTenSuccess.push_front(true);
  } else
    _lastTenSuccess.push_front(false);

  if (_lastTenLines.size() > 10) {
    _lastTenLines.pop_back();
    _lastTenSuccess.pop_back();
  }
}
clf::MonitoringData::MonitoringData(std::shared_ptr<clf::Config> cfg)
    : _versionArray{0, 0, 0}, _cfg(cfg), _totalLines{0}, _totalValidLines{0},
      _startTime(std::chrono::system_clock::now()) {
}

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
    this->_totalSize = data.totalSize();

    _orderedVerb.clear();
    _orderedStatus.clear();
    _orderedPath.clear();
    _verbMap.clear();
    _statusMap.clear();
    _pathMap.clear();

    for (auto &v : data._verbMap)
      this->_orderedVerb.emplace(v.second, v.first);

    for (auto &s : data._statusMap)
      this->_orderedStatus.emplace(s.second, s.first);

    for (auto &p : data._pathMap)
      this->_orderedPath.emplace(p.second, p.first);
  }
  return *this;
}
std::string clf::MonitoringData::cfg() const {
  auto replaceAll = [](std::string str, const std::string &from,
                       const std::string &to) -> std::string {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      start_pos +=
          to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
  };

  return replaceAll(_cfg->dumpToJson(), ",", ",\n");
}
uint64_t clf::MonitoringData::totalSize() const { return _totalSize; }

clf::GlobalMonitoringData::GlobalMonitoringData(
    std::shared_ptr<clf::Config> cfg)
    : MonitoringData(cfg), _alertOn{false} {}

deque<std::pair<clf::Timepoint, std::string>> &
clf::GlobalMonitoringData::lastTenLines() {
  return _lastTenLines;
}

std::deque<bool> &clf::GlobalMonitoringData::lastTenSuccess() {
  return _lastTenSuccess;
}

std::unordered_map<boost::beast::http::verb, uint64_t> &
clf::MonitoringData::verbMap() {
  return _verbMap;
}

std::unordered_map<boost::beast::http::status, uint64_t> &
clf::MonitoringData::statusMap() {
  return _statusMap;
}

std::unordered_map<std::string, uint64_t> &clf::MonitoringData::pathMap() {
  return _pathMap;
}

std::array<uint64_t, 3> &clf::MonitoringData::versionArray() {
  return _versionArray;
}

std::map<uint64_t, boost::beast::http::verb> &
clf::MonitoringData::orderedVerbMap() {
  return _orderedVerb;
}

std::map<uint64_t, boost::beast::http::status> &
clf::MonitoringData::orderedStatusMap() {
  return _orderedStatus;
}

std::map<uint64_t, std::string> &clf::MonitoringData::orderedPathMap() {
  return _orderedPath;
}

bool clf::GlobalMonitoringData::alertOn() const { return _alertOn; }

vector<clf::Timepoint> &clf::GlobalMonitoringData::alertTs() {
  return _alertTs;
}

void clf::GlobalMonitoringData::checkAlarm() {
  _alertTs.erase(std::remove_if(_alertTs.begin(), _alertTs.end(),
                                [&](Timepoint &tp) {
                                  if ((tp + _cfg->alertTimeMs()) <
                                      std::chrono::system_clock::now()) {
                                    return true;
                                  }

                                  return false;
                                }),
                 _alertTs.end());

  if (_alertTs.size() > _cfg->alertThresholdNumber()) {
    if (!_alertOn)
      _startTime = _alertTs.front();
    _alertOn = true;
  } else {
    _alertOn = false;
    _hits = 0;
  };
}
uint64_t clf::GlobalMonitoringData::hits() const { return _hits; }

clf::Data::Data(std::shared_ptr<clf::Config> cfg)
    : _globalData(cfg), _currentFrameData(cfg), _lastFrameFrameData(cfg) {}
