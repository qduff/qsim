#include "overlay.h"
#define TRACY_ENABLE
#include "Tracy.hpp"
#include <istream>
#include <vector>

bool my_tool_active;

void renderFPSbox(kernelInterface &ki, float frametime, int fps) {
  ZoneScoped;
  static int location = 0;
  ImGuiIO &io = ImGui::GetIO();
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav;
  if (location >= 0) {
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos =
        viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x =
        (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y =
        (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
  } else if (location == -2) {
    // Center window
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    window_flags |= ImGuiWindowFlags_NoMove;
  }
  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
  if (ImGui::Begin("Example: Simple overlay", NULL, window_flags)) {
    ImGui::Text("Game Info");
    ImGui::Separator();
    ImGui::Indent(16.0f);
    ImGui::Text("Frame time: %fms", frametime * 1000);
    ImGui::Text("FPS: %i", fps);

    ImGui::Unindent(16.0f);

    ImGui::Text("Sim info");
    ImGui::Separator();
    ImGui::Indent(16.0f);
    ImGui::Text("Sim time: %fs", (float)ki.shmem->micros_passed / 1000000.0);
    ImGui::Text("Cycle time %fms", (float)ki.shmem->nanos_cycle / 1000000.0f);
    ImGui::Text("Cycle rate %fHz",
                1 / ((float)ki.shmem->nanos_cycle / 1000000000.0f));

    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Custom", NULL, location == -1))
        location = -1;
      if (ImGui::MenuItem("Center", NULL, location == -2))
        location = -2;
      if (ImGui::MenuItem("Top-left", NULL, location == 0))
        location = 0;
      if (ImGui::MenuItem("Top-right", NULL, location == 1))
        location = 1;
      if (ImGui::MenuItem("Bottom-left", NULL, location == 2))
        location = 2;
      if (ImGui::MenuItem("Bottom-right", NULL, location == 3))
        location = 3;
      // if (NULL && ImGui::MenuItem("Close")) *NULL = false;
      ImGui::EndPopup();
    }
  }
  ImGui::End();
}

void renderOverlay(kernelInterface &ki) {
  ZoneScoped;
  ImGui::Begin("INTERFACE TOOLS", &my_tool_active, ImGuiWindowFlags_MenuBar);
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("TOOLS")) {
      if (ImGui::MenuItem("Kill", "-9")) {
        kill(ki.pid, 11);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  ImGui::BeginChild("Scrolling");

  ImGui::SetNextItemOpen(true, ImGuiCond_Once);

  if (ImGui::TreeNode("Info ")) {
    ImGui::Text("%i.%i.%i: Schema ver", ki.shmem->schemaVersion[0],
                ki.shmem->schemaVersion[1], ki.shmem->schemaVersion[2]);
    ImGui::Text("%i.%i.%i: parent ver", ki.shmem->parentVersion[0],
                ki.shmem->parentVersion[1], ki.shmem->parentVersion[2]);
    ImGui::Text("%i.%i.%i: child ver", ki.shmem->childVersion[0],
                ki.shmem->childVersion[1], ki.shmem->childVersion[2]);

    ImGui::TreePop();
  }
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);

  if (ImGui::TreeNode("Child ")) {

    ImGui::Text("%04d: PID", ki.pid);
    ImGui::Text("%p: Shared memory addr", ki.shmem);
    ImGui::Text("%lu: Shared memory size", sizeof(*ki.shmem));

    ImGui::Text("%s (%d)", ki.isRunning ? "Running" : "DEAD", ki.wstatus);

    ImGui::TreePop();
  }

  ImGui::EndChild();
  ImGui::End();
}

void showTexture(void *texture, int width, int height) {
  ImGui::Begin("OpenGL Texture Text");
  ImGui::Text("pointer = %p", texture);
  ImGui::Text("size = %d x %d", width, height);
  ImGui::Image((void *)(intptr_t)texture, ImVec2(width, height));
  ImGui::End();
}

void renderOSDOverlay(osdRenderer &osd) {
  ZoneScoped;
  ImGui::Begin("OSD overlay");

      std::vector<std::string> fonts = osd.getOSDFonts();


  if (ImGui::BeginListBox("##listbox 1")){
for (int n = 0; n < fonts.size(); n++) {
    if (ImGui::Selectable(fonts[n].c_str(), false)) {
      osd.changeOSDFont(fonts[n]);
    }
  }
  ImGui::EndListBox();
  }
  

  ImGui::End();
}
