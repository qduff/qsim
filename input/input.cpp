#include "input.hpp"

#include "../libraries/imgui/imgui.h"

bool active = true;

//! TODO CLAMP1!!

void Input::processAxes(engineInterface &ki, const char *name,
                        const float *axes, int count) {

  for (int i = 0; i < 16; ++i) { // iterate over OUTPUTs
    if (rcmapping.size() > i) {
      float val = axes[std::get<0>(rcmapping[i])] * std::get<2>(rcmapping[i]) + std::get<1>(rcmapping[i]);
      ki.writeAxis(val, i);
      // printf("SENDOUT %i :  %f\n", i, shmem->rc[i]);
    } else {
      ki.writeAxis(0, i);
    }
  }
    present = true;
  lastaxes = axes;
  lastcount =count;

}


void Input::renderOverlay(){
  ImGui::Begin("input", &active);
  if (present){

    for (int i; i<rcmapping.size(); ++i){
      ImGui::Text("Output Channel %i",i);

      int *source =&std::get<0>(rcmapping[i]);
      float *offset = &std::get<1>(rcmapping[i]);
      float *scale = &std::get<2>(rcmapping[i]);

      printf("out:%i source:%i\n",i, *source);
      printf("source = %p\n", source);


      ImGui::SliderInt(("Source for "+std::to_string(i)).c_str(),source, 0, lastcount);
      ImGui::Text("Raw input channel %i",*source);

      float raw = lastaxes[*source];
        char buf[32];
        sprintf(buf, "%i", (int)(raw *100));
      ImGui::ProgressBar((raw+1)/2, ImVec2(0.0f, 0.0f), buf);


      // static float f1 = 0.123f;
      float vec4f[] = { *offset, *scale };
      ImGui::SliderFloat2(("off,sca" +std::to_string(i)).c_str(), vec4f, -1.0f, 1.0f);
      *offset = vec4f[0];
      *scale = vec4f[1];


      float val = lastaxes[*source] * *scale + *offset;
      sprintf(buf, "%i", (int)(val *100));
      ImGui::ProgressBar((val+1)/2, ImVec2(0.0f, 0.0f), buf);


    };

  }else{
    ImGui::Text("No Controller present!");
  }

  ImGui::End();


}