#pragma once

#include "../shader.hpp"
#include <glad/glad.h>
#include <string>
#include <vector>
// #define STB_IMAGE_IMPLEMENTATION
// #include "../utils/stb_image.h"

class sceneRenderer {

private:
  Shader cubeShader{"../resources/shaders/testing/cube.vert",
                    "../resources/shaders/testing/cube.frag"};

  unsigned int VBO, VAO;
  unsigned int texture1, texture2;


public:
  sceneRenderer();
  ~sceneRenderer();

  void render(int width, int height);
};