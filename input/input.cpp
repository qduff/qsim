#include "input.hpp"

#include "../libraries/imgui/imgui.h"

bool active = true;

//! TODO CLAMP1!!, message if so!
// spin out final from input into own func

void Input::processAxes(engineInterface &ki) {

  for (int i = 0; i < 16; ++i) { // iterate over OUTPUTs
    if (rcmapping.size() > i) {
      ki.writeAxis(getProcessed(i), i);
    } else {
      ki.writeAxis(0, i);
    }
  }
}

void Input::renderOverlay() {
  char buf[32];
  ImGui::Begin("input", &active);
  if (present) {
    ImGui::Text("NAME: %s\n", name);
    ImGui::Text("Channels: %d\n", axescount);
    ImGui::Text("Buttons(wip): %d\n", buttoncount);

    for (int i = 0; i < rcmapping.size(); ++i) {
      sprintf(buf, "Output Channel %i", i);
      ImGui::SeparatorText(buf);

      ImGui::SliderInt(("Source for " + std::to_string(i)).c_str(),
                       getSource(i), 0, axescount - 1);
      ImGui::Text("Raw input channel %i", *getSource(i));

      float raw = getAxisValue(*getSource(i));

      sprintf(buf, "%i", (int)(raw * 100));
      ImGui::ProgressBar((raw + 1) / 2, ImVec2(0.0f, 0.0f), buf);

      // static float f1 = 0.123f;
      float vec4f[] = {*getOffset(i), *getDeadzone(i), *getScale(i)};
      ImGui::SliderFloat3(("off,ded,sca" + std::to_string(i)).c_str(), vec4f,
                          -1.0f, 1.0f);
      *getOffset(i) = vec4f[0];
      *getDeadzone(i) = vec4f[1];
      *getScale(i) = vec4f[2];
      // ORDER: DSO
      float val = getProcessed(i);
      sprintf(buf, "%i", (int)(val * 100));
      ImGui::ProgressBar((val + 1) / 2, ImVec2(0.0f, 0.0f), buf);
    };
  } else {
    ImGui::Text("No Controller present!");
  }

  ImGui::End();
}