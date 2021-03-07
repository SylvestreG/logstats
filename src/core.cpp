//
// Created by syl on 3/3/21.
//

#include <spdlog/spdlog.h>
#include <imtui/imtui.h>
#include <imtui/imtui-impl-ncurses.h>

#include "core.h"


Core::Core(std::shared_ptr<clf::Config> cfg, std::filesystem::path const &path)
    : _config{cfg}, _fileWatcher(cfg, path),
      _timer(_ioCtx, cfg->refreshTimeMs()) {
  _fileWatcher.setNewBufferCallback(
      [&](std::vector<std::pair<clf::Timepoint, std::string>> &&buffers) {
        this->onNewBuffer(std::move(buffers));
      });
  _splitter.setNewLineCallback(
      [&](std::string &&line) { spdlog::info("get new line {}", line); });
}

Core::~Core() {

}

void Core::run() {
  _timer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });

  _splitter.start();
  _fileWatcher.start();

  _asioThread = std::thread([&]() { _ioCtx.run(); });

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto screen = ImTui_ImplNcurses_Init(true);
  ImTui_ImplText_Init();

  bool demo = true;
  int nframes = 0;
  float fval = 1.23f;
  bool quit{false};
  while (!quit) {
    ImTui_ImplNcurses_NewFrame();
    ImTui_ImplText_NewFrame();

    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(60.0, 10.0) , ImGuiCond_Once);


    ImGui::Begin("Global Data");
    if (ImGui::Button("Quit")) {
      quit = true;
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, 12.0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(60.0, 10.0) , ImGuiCond_Once);

    ImGui::Begin("Monitoring Window");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, 24.0) , ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(60.0, 10.0) , ImGuiCond_Once);

    ImGui::Begin("Alerting");
    ImGui::End();

    ImGui::Render();

    ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), screen);
    ImTui_ImplNcurses_DrawScreen();
  }
  ImTui_ImplText_Shutdown();
  ImTui_ImplNcurses_Shutdown();

  spdlog::info("stopping ioctx");
  _ioCtx.stop();
  _asioThread.join();

  spdlog::info("stopping filewatcher");
  _fileWatcher.stop();

  spdlog::info("stopping splitter");
  _splitter.stop();
  spdlog::info("clfParser done");
}

void Core::onNewBuffer(
    std::vector<std::pair<clf::Timepoint, std::string>> &&buffers) {
  _splitter.pushIntoSplitter(std::move(buffers));
}

void Core::refreshDisplayCallback() {

  _timer.expires_at(_timer.expiry() + _config->refreshTimeMs());
  _timer.async_wait(
      [&](boost::system::error_code const &) { refreshDisplayCallback(); });
}
