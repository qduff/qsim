#pragma once
#include "../shader.hpp"
#include "../memdef.h"

#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class sceneRenderer {

private:
  Shader cubeShader{"../resources/shaders/testing/cube.vert",
                    "../resources/shaders/testing/cube.frag"};

  unsigned int VBO, VAO;
  unsigned int texture1, texture2;

public:
  sceneRenderer();
  ~sceneRenderer();

  void render(memory_s *shmem, int width, int height);
};