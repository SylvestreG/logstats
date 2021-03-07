//
// Created by sylvestre on 07/03/2021.
//

#ifndef LOGSTATS_UI_H
#define LOGSTATS_UI_H

namespace clf {
class Ui {
public:
  Ui() = default;
  ~Ui() = default;
  Ui(Ui const&) = delete;
  Ui& operator=(Ui const&) = delete;

  void updateDisplay(void);

private:
};
} // namespace clf

#endif // LOGSTATS_UI_H
