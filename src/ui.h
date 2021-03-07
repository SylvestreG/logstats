//
// Created by syl on 3/7/21.
//

#ifndef LOGSTATS_UI_H
#define LOGSTATS_UI_H

#include "data.h"

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
  void renderAlerts();

  clf::Data &_data;
};
};     // namespace clf
#endif // LOGSTATS_UI_H
