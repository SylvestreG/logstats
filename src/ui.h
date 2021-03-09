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
#ifndef __UI_H__
#define __UI_H__

#include "data.h"

namespace clf {
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
#endif // __UI_H__
