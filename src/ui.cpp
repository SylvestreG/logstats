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
#include <fmt/color.h>
#include <imtui/imtui-impl-ncurses.h>
#include <imtui/imtui.h>
#include <spdlog/spdlog.h>

#include "ui.h"

clf::Ui::Ui(clf::Data &d) : _data(d), _cfg(d._currentFrameData.cfg()) {}

void clf::Ui::render() {

  auto findNbVerb = [&](boost::beast::http::verb v) -> uint64_t {
    auto it = _data._globalData.verbMap().find(v);

    if (it == _data._globalData.verbMap().end())
      return 0;

    return it->second;
  };

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto screen = ImTui_ImplNcurses_Init(true);
  ImTui_ImplText_Init();

  bool quit{false};
  while (!quit) {
    ImTui_ImplNcurses_NewFrame();
    ImTui_ImplText_NewFrame();

    ImGui::NewFrame();

    {
      std::lock_guard<std::mutex> lck(_data._dataMutex);
      _renderData.totalValidLines = _data._globalData.totalValidLines();
      _renderData.totalLines = _data._globalData.totalLines();
      _renderData.nbRequestInTimeframe = _data._globalData.alertTs().size();
      _renderData.nbGet = findNbVerb(boost::beast::http::verb::get);
      _renderData.nbPost = findNbVerb(boost::beast::http::verb::post);
      _renderData.nbPut = findNbVerb(boost::beast::http::verb::put);
      _renderData.nbPatch = findNbVerb(boost::beast::http::verb::patch);
      _renderData.nbDelete = findNbVerb(boost::beast::http::verb::delete_);
      _renderData.nbUnknown = findNbVerb(boost::beast::http::verb::unknown);
      _renderData._alert = _data._globalData.alertOn();
      _renderData.nbHits = _data._globalData.hits();
      _renderData.totalSize = _data._globalData.totalSize();

      if (_data._globalData.lastRecover()) {
        _renderData._lastRecoverOn = true;
        std::time_t now_c = std::chrono::system_clock::to_time_t(
            *_data._globalData.lastRecover());
        std::tm now_tm = *std::localtime(&now_c);
        std::strftime(_renderData.lastRecover, 80, "%d/%b/%Y:%H:%M:%S %z",
                      &now_tm);
      } else {
        _renderData._lastRecoverOn = false;
      }

      _renderData.currentTotalLines = _data._lastFrameFrameData.totalLines();
      _renderData.currentTotalValidLines =
          _data._lastFrameFrameData.totalValidLines();
      _renderData.currentTotalSize = _data._lastFrameFrameData.totalSize();

      _renderData.lastUpdateSec =
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now() -
              _data._currentFrameData.startTime());

      if (_renderData._alert) {
        std::time_t now_c =
            std::chrono::system_clock::to_time_t(_data._globalData.startTime());
        std::tm now_tm = *std::localtime(&now_c);
        std::strftime(_renderData.timeStr, 80, "%d/%b/%Y:%H:%M:%S %z", &now_tm);
      }
    }
    renderGlobalStats();
    renderMonitoringStats();
    quit = renderMenu();

    ImGui::Render();

    ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
    ImTui_ImplNcurses_DrawScreen();
  }
  ImTui_ImplText_Shutdown();
  ImTui_ImplNcurses_Shutdown();
}

bool clf::Ui::renderMenu() {
  std::string s =
      R"foo(     _=,_
    o_/6 /#\
    \__ |##/
     ='|--\
       /   #'-.
       \#|_   _'-. /
       |/ \_( # |"
      C/ ,--___/)foo";
  bool quit{false};

  ImGui::SetNextWindowPos(ImVec2(60, 13.0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(20.0, 12.0), ImGuiCond_Once);

  ImGui::Begin("MENU");
  if (_renderData._alert)
    ImGui::TextColored(ImVec4(1.0, 0.1, 0.1, 1.0), "%s", s.c_str());
  else
    ImGui::TextColored(ImVec4(0.5, 0.0, 1.0, 1.0), "%s", s.c_str());
  if (ImGui::Button("QUIT")) {
    quit = true;
  }
  ImGui::End();

  return quit;
}

void clf::Ui::renderGlobalStats() {
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(80.0, 13.0), ImGuiCond_Once);

  ImGui::Begin("Global Data");
  if (_renderData._alert) {
    ImGui::TextColored(ImVec4(1.00, 0.0, 0.0, 1.0),
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    ImGui::TextColored(
        ImVec4(1.00, 0.0, 0.0, 1.0), "%s",
        fmt::format(
            "High traffic generated an alert - hits = {}, triggered at {}",
            _renderData.nbHits, _renderData.timeStr)
            .c_str());
    ImGui::TextColored(ImVec4(1.00, 0.0, 0.0, 1.0),
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  } else if (_renderData._lastRecoverOn) {
    ImGui::TextColored(ImVec4(0.00, 1.0, 0.0, 1.0),
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    ImGui::TextColored(
        ImVec4(0.00, 1.0, 0.0, 1.0), "%s",
        fmt::format("Alert recovered at {}", _renderData.lastRecover).c_str());
    ImGui::TextColored(ImVec4(0.00, 1.0, 0.0, 1.0),
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                       "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  }
  if (ImGui::BeginTabBar("info", ImGuiTabBarFlags_None)) {
    if (ImGui::BeginTabItem("stats")) {
      ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                         fmt::format("Valid request {}/{}",
                                     _renderData.totalValidLines,
                                     _renderData.totalLines)
                             .c_str());
      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                         fmt::format("request in alert timeframe {}",
                                     _renderData.nbRequestInTimeframe)
                             .c_str());
      ImGui::Separator();
      ImGui::TextColored(
          ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
          fmt::format("total size {}", printSize(_renderData.totalSize))
              .c_str());
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("verbs")) {
      ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                         fmt::format("GET ({})", _renderData.nbGet).c_str());
      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                         fmt::format("POST ({})", _renderData.nbPost).c_str());
      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                         fmt::format("PUT ({})", _renderData.nbPut).c_str());
      ImGui::Separator();
      ImGui::TextColored(
          ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
          fmt::format("PATCH ({})", _renderData.nbPatch).c_str());
      ImGui::Separator();
      ImGui::TextColored(
          ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
          fmt::format("DELETE ({})", _renderData.nbDelete).c_str());
      ImGui::Separator();
      ImGui::TextColored(
          ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
          fmt::format("UNKNOWN ({})", _renderData.nbUnknown).c_str());

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("lines")) {
      {
        std::lock_guard<std::mutex> lck(_data._dataMutex);

        int i{0};
        ImVec4 green{0.0, 1.0, 0.0, 1.0};
        ImVec4 red{1.0, 0.0, 0.0, 1.0};
        for (auto &s : _data._globalData.lastTenLines()) {
          std::time_t now_c = std::chrono::system_clock::to_time_t(s.first);
          std::tm now_tm = *std::localtime(&now_c);
          std::strftime(_renderData.timeStr, 80, "%d/%b/%Y:%H:%M:%S %z",
                        &now_tm);
          ImGui::TextColored(
              (_data._globalData.lastTenSuccess()[i]) ? green : red, "%s",
              fmt::format("{} | {}", _renderData.timeStr, s.second).c_str());
          i++;
        }
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("config")) {
      ImGui::Text("%s", _cfg.c_str());
      ImGui::EndTabItem();
    }
  }
  ImGui::EndTabBar();
  ImGui::End();
}

void clf::Ui::renderMonitoringStats() {
  ImGui::SetNextWindowPos(ImVec2(0, 13.0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(60.0, 12.0), ImGuiCond_Once);

  ImGui::Begin("monitoring");
  ImGui::Text("%s", fmt::format("{} secs since last refresh",
                                _renderData.lastUpdateSec.count())
                        .c_str());
  if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {
    if (ImGui::BeginTabItem("stats")) {
      ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                         fmt::format("Valid request {}/{}",
                                     _renderData.currentTotalValidLines,
                                     _renderData.currentTotalLines)
                             .c_str());
      ImGui::Separator();
      ImGui::TextColored(
          ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
          fmt::format("total size {}", printSize(_renderData.currentTotalSize))
              .c_str());
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("verbs")) {
      std::lock_guard<std::mutex> lck(_data._dataMutex);
      for (auto it = _data._lastFrameFrameData.orderedVerbMap().rbegin();
           it != _data._lastFrameFrameData.orderedVerbMap().rend(); ++it)
        ImGui::Text(
            "%s",
            fmt::format(
                "{} ({})",
                boost::beast::http::to_string(it->second).to_string().c_str(),
                it->first)
                .c_str());

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("status")) {
      std::lock_guard<std::mutex> lck(_data._dataMutex);
      for (auto it = _data._lastFrameFrameData.orderedStatusMap().rbegin();
           it != _data._lastFrameFrameData.orderedStatusMap().rend(); ++it)
        ImGui::Text("%s",
                    fmt::format("{} ({})",
                                boost::beast::http::obsolete_reason(it->second)
                                    .to_string()
                                    .c_str(),
                                it->first)
                        .c_str());
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("paths")) {
      for (auto it = _data._lastFrameFrameData.orderedPathMap().rbegin();
           it != _data._lastFrameFrameData.orderedPathMap().rend(); ++it)
        ImGui::Text(
            "%s",
            fmt::format("{} ({})", it->second.c_str(), it->first).c_str());
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
  }
}

std::string clf::Ui::printSize(uint64_t total) {
  uint16_t tera;
  uint16_t giga;
  uint16_t mega;
  uint16_t kilo;
  uint16_t oct;
  std::string result;

  tera = total / (1024ull * 1024ull * 1024ull * 1024ull);
  total = total % (1024ull * 1024ull * 1024ull * 1024ull);
  giga = total / (1024 * 1024 * 1024);
  total = total % (1024 * 1024 * 1024);
  mega = total / (1024 * 1024);
  total = total % (1024 * 1024);
  kilo = total / 1024;
  total = total % (1024);
  oct = total;

  if (tera)
    result.append(fmt::format("{}t", tera));
  if (giga)
    result.append(fmt::format("{}g", giga));
  if (mega)
    result.append(fmt::format("{}m", mega));
  if (kilo)
    result.append(fmt::format("{}k", kilo));
  result.append(fmt::format("{}", oct));

  return result;
}
