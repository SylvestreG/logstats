//
// Created by syl on 3/7/21.
//

#ifndef LOGSTATS_UI_H
#define LOGSTATS_UI_H

#include "data.h"

// copy of all data for a render phase to avoid locks
struct DataRendering {
  char timeStr[80];
  uint64_t nbGet, nbPost, nbPut, nbPatch, nbDelete, nbUnknown;
  uint64_t nbRequestInTimeframe;
  uint64_t totalValidLines;
  uint64_t totalLines;
  uint64_t totalSize;
  uint64_t nbHits;
  bool _alert;

  uint64_t currentTotalValidLines;
  uint64_t currentTotalLines;
  uint64_t currentTotalSize;

  std::chrono::seconds lastUpdateSec;
};

namespace clf {
class Ui {
public:
  Ui(clf::Data &);
  ~Ui() = default;
  Ui(Ui const &) = delete;

  Ui &operator=(Ui const &) = delete;

  void render();

private:
  bool renderMenu();
  void renderGlobalStats();
  void renderMonitoringStats();
  std::string printSize(uint64_t);

  clf::Data &_data;
  std::string _cfg;

  DataRendering _renderData;
};
};     // namespace clf
#endif // LOGSTATS_UI_H
