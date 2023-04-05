// clang-format off
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "libraries/imgui/imgui.h"
#include "libraries/imgui/imgui_impl_glfw.h"
#include "libraries/imgui/imgui_impl_opengl3.h"
// clang-format on

#include "utils/loadmcm.h"

#include "kernelinterface.hpp"
#include "shader.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

kernelInterface ki;

void renderOSD(Shader &s, uint8_t *osdpnt, unsigned int texture,
               unsigned int VAO, unsigned int VBO) {
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindTexture(GL_TEXTURE_2D, texture);
  s.use();
  const int width = 30;
  const int height = 16;
  for (char y = 0; y < 16; ++y) { //
    for (char x = 0; x < 30; ++x) {
      uint8_t curchar = *(osdpnt + (15 - y) * width + x);
      // if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
      //   curchar = 0x2a; // asterisk
      // }
      // curchar = y * width + x;
      // printf("x%iy%i - %c \n", x, y, curchar);
      float tex_xl = (float)curchar / 256;
      float tex_xr = ((float)curchar + 1) / 256;

      float ryu = -1 + 2 * ((float)(y) / height);
      float ryd = -1 + 2 * ((float)(y + 1) / height);
      float rxl = -1 + 2 * ((float)(x) / width);
      float rxr = -1 + 2 * ((float)(x + 1) / width);
      // printf("y:%f -> %f", ryd, ryu);
      //* it is important to note that these have to be rendered
      //* flipped in the Y-AXIS
      float vertices[] = {
          rxr, ryu, 0.0f, tex_xr, 1.0f, // top right
          rxr, ryd, 0.0f, tex_xr, 0.0f, // bottom right
          rxl, ryd, 0.0f, tex_xl, 0.0f, // bottom left
          rxl, ryu, 0.0f, tex_xl, 1.0f  // top left
      };
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                   GL_DYNAMIC_DRAW);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }
}

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "uhh title... yes.", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(0);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }



  // configure imgui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    bool show_demo_window = true;






  bool i = ki.start();
  printf("start:%d\n", i);
  printf("start:%d\n", ki.isRunning());


  // build and compile our shader zprogram
  // ------------------------------------
  Shader ourShader("../shaders/shader.vert", "../shaders/shader.frag");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  // float vertices[] = {
  //     // positions                    // texture coords
  //     0.5f,  0.5f,  0.0f, 7. / 256, 1.0f, // top right
  //     0.5f,  -0.5f, 0.0f, 7. / 256, 0.0f, // bottom right
  //     -0.5f, -0.5f, 0.0f, 0.0f,     0.0f, // bottom left
  //     -0.5f, 0.5f,  0.0f, 0.0f,     1.0f  // top left
  // };
  // unsigned int indices[] = {
  //     0, 1, 3, // first triangle
  //     1, 2, 3  // second triangle
  // };
  unsigned int VBO, VAO, EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float), NULL, GL_DYNAMIC_DRAW);
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (GLvoid *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);

  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
  // GL_STATIC_DRAW);

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  // position attribute
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void
  // *)0); glEnableVertexAttribArray(0);
  // // color attribute
  // // texture coord attribute
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
  //                       (void *)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);

  // load and create a texture
  // -------------------------
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height, nrChannels;

  unsigned char *data =
      loadMCM("../resources/osdfonts/default.mcm", width, height);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  delete data;

  // end tex

  printf("%p", ki.osdpnt);
  // render loop
  // -----------
  double lasttime = glfwGetTime();
  int frameslasts = 0;
  float fps = 0;





  // GAME LOOP

  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    processInput(window);

    // ki.debugOsdPrint();

    // render
    // ------
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();


  ImGui::ShowDemoWindow(&show_demo_window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    double curtime = glfwGetTime();
    if (curtime - lasttime >= 1) {
      // printf("%f ms/frame\n", 1000.0 / double(frameslasts));
      printf("frames: %i\n", frameslasts);
      frameslasts = 0;
      fps = frameslasts;
      lasttime = curtime;
    } else {
      frameslasts += 1;
    }

    renderOSD(ourShader, ki.osdpnt, texture, VAO, VBO);
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse
    // moved etc.)
    // -------------------------------------------------------------------------------


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
    int count;
    const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
    ki.writeAxes(axes, count);
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina
  // displays.
  glViewport(0, 0, width, height);
}