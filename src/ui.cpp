//
// Created by syl on 3/7/21.
//

#include <fmt/color.h>
#include <imtui/imtui-impl-ncurses.h>
#include <imtui/imtui.h>
#include <spdlog/spdlog.h>

#include "ui.h"

clf::Ui::Ui(clf::Data &d) : _data(d) {}

void clf::Ui::render() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto screen = ImTui_ImplNcurses_Init(true);
  ImTui_ImplText_Init();

  bool quit{false};
  while (!quit) {
    ImTui_ImplNcurses_NewFrame();
    ImTui_ImplText_NewFrame();

    ImGui::NewFrame();

    renderGlobalStats();
    renderMonitoringStats();
    renderAlerts();
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

  ImGui::SetNextWindowPos(ImVec2(120, 30.0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(20.0, 10.0), ImGuiCond_Once);

  ImGui::Begin("MENU");
  ImGui::TextColored(ImVec4(0.5, 0.0, 1.0, 1.0), "%s", s.c_str());
  if (ImGui::Button("QUIT")) {
    quit = true;
  }
  ImGui::End();

  return quit;
}

void clf::Ui::renderGlobalStats() {
  ImGui::SetNextWindowPos(ImVec2(100, 0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(60.0, 13.0), ImGuiCond_Once);

  ImGui::Begin("Global Data");
  ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                     fmt::format("last 10 messages:   Valid {}/{}\n",
                                 _data._totalValidLines, _data._totalLines)
                         .c_str());

  {
    std::lock_guard<std::mutex> lck(_data._dataMutex);

    int i{0};
    for (auto &s : _data._lastTenLines) {
      if (_data._lastTenSuccess[i])
        ImGui::TextColored(
            ImVec4(0.0, 1.0, 0.0, 1.0), "%s",
            fmt::format("{} | {}",
                        std::chrono::duration_cast<std::chrono::seconds>(
                            s.first - _data._appStartTime)
                            .count(),
                        s.second)
                .c_str());
      else
        ImGui::TextColored(
            ImVec4(1.0, 0.0, 0.0, 1.0), "%s",
            fmt::format("{} | {}",
                        std::chrono::duration_cast<std::chrono::seconds>(
                            s.first - _data._appStartTime)
                            .count(),
                        s.second)
                .c_str());
      i++;
    }
  }
  ImGui::End();
}

void clf::Ui::renderMonitoringStats() {
  ImGui::SetNextWindowPos(ImVec2(0, 12.0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(60.0, 10.0), ImGuiCond_Once);

  ImGui::Begin("Monitoring Window");
  ImGui::End();
}

void clf::Ui::renderAlerts() {
  std::string warn =
      R"foo(  / \
 / ! \
|-----|
)foo";

  ImGui::SetNextWindowPos(ImVec2(0, 24.0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(60.0, 10.0), ImGuiCond_Once);

  ImGui::Begin("Alerting");
  ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%s", warn.c_str());
  ImGui::End();
}
