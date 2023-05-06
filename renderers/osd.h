#pragma once

#include "../memdef.h"
#include "../shader.hpp"
#include "../utils/loadmcm.h"
#include "TracyOpenGL.hpp"
#include <glad/glad.h>
#include <vector>

class osdRenderer {

private:
  Shader osdShader =
    Shader("../resources/shaders/osd.vert", "../resources/shaders/osd.frag");
  unsigned int VAO_osd;
  unsigned int VBO_osd;
  unsigned int EBO_osd;
  unsigned int OSDTEX_osd;

  void loadTex(std::string fontname);

public:
  osdRenderer(std::string fontname);
  ~osdRenderer();

  std::vector<std::string> getOSDFonts();
  void changeOSDFont(std::string fontname);
  void renderOSD(memory_s *shmem, int width, int height);
  void renderOSDStupidly(memory_s *shmem, int width, int height);

};