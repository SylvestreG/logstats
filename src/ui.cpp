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

  uint64_t nbGet, nbPost, nbPut, nbPatch, nbDelete, nbUnknown;
  uint64_t totalValidLines;
  uint64_t totalLines;
  uint64_t totalSize;

  auto findNbVerb = [&](boost::beast::http::verb v) -> uint64_t {
    auto it = _data._globalData.verbMap().find(v);

    if (it == _data._globalData.verbMap().end())
      return 0;

    return it->second;
  };

  {
    std::lock_guard<std::mutex> lck(_data._dataMutex);
    totalValidLines = _data._globalData.totalValidLines();
    totalLines = _data._globalData.totalLines();
    totalSize = 0;
    nbGet = findNbVerb(boost::beast::http::verb::get);
    nbPost = findNbVerb(boost::beast::http::verb::post);
    nbPut = findNbVerb(boost::beast::http::verb::put);
    nbPatch = findNbVerb(boost::beast::http::verb::patch);
    nbDelete = findNbVerb(boost::beast::http::verb::delete_);
    nbUnknown = findNbVerb(boost::beast::http::verb::unknown);
  }

  ImGui::Begin("Global Data");
  if (ImGui::BeginTabBar("info", ImGuiTabBarFlags_None)) {
    if (ImGui::BeginTabItem("stats")) {
      {
        ImGui::TextColored(
            ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
            fmt::format("Valid request {}/{}", totalValidLines, totalLines)
                .c_str());
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                           fmt::format("total size {}", totalSize).c_str());
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("verbs")) {
      {
        ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                           fmt::format("GET ({})", nbGet).c_str());
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                           fmt::format("POST ({})", nbPost).c_str());
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                           fmt::format("PUT ({})", nbPut).c_str());
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                           fmt::format("PATCH ({})", nbPatch).c_str());
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.50, 1.0, 0.8, 1.0), "%s",
                           fmt::format("DELETE ({})", nbDelete).c_str());
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.59, 1.0, 0.8, 1.0), "%s",
                           fmt::format("UNKNOWN ({})", nbUnknown).c_str());
      }

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("stats")) {
      {
        std::lock_guard<std::mutex> lck(_data._dataMutex);

        int i{0};
        ImVec4 green{0.0, 1.0, 0.0, 1.0};
        ImVec4 red{1.0, 0.0, 0.0, 1.0};
        for (auto &s : _data._globalData.lastTenLines()) {
          ImGui::TextColored(
              (_data._globalData.lastTenSuccess()[i]) ? green : red, "%s",
              fmt::format("{} | {}",
                          std::chrono::duration_cast<std::chrono::seconds>(
                              s.first - _data._globalData.startTime())
                              .count(),
                          s.second)
                  .c_str());
          i++;
        }
      }
      ImGui::EndTabItem();
    }
  }
  ImGui::EndTabBar();
  ImGui::End();
}

void clf::Ui::renderMonitoringStats() {
  ImGui::SetNextWindowPos(ImVec2(0, 12.0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(60.0, 10.0), ImGuiCond_Once);

  ImGui::Begin("monitoring");
  if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {
    if (ImGui::BeginTabItem("Avocado")) {
      ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Broccoli")) {
      ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Cucumber")) {
      ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
  }
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
  ImGui::Text("Without border:");
  ImGui::Columns(3, "mycolumns3", false); // 3-ways, no border
  ImGui::Separator();
  for (int n = 0; n < 14; n++) {
    char label[32];
    sprintf(label, "Item %d", n);
    if (ImGui::Selectable(label)) {
    }
    // if (ImGui::Button(label, ImVec2(-FLT_MIN,0.0f))) {}
    ImGui::NextColumn();
  }
  ImGui::Columns(1);
  ImGui::Separator();
  ImGui::End();
}
