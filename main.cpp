// clang-format off
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TRACY_ENABLE
// #define TRACY_NO_FRAME_IMAGE
#include "Tracy.hpp"
#include "TracyOpenGL.hpp"


#include "libraries/imgui/imgui.h"
#include "libraries/imgui/imgui_impl_glfw.h"
#include "libraries/imgui/imgui_impl_opengl3.h"

#include "overlay/overlay.h"
// clang-format on

#include "memdef.h"
#include "preferences.h"

#include "kernelinterface.hpp"
#include "shader.hpp"

#include "renderers/osd.h"

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

kernelInterface ki;


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
  glfwSwapInterval(1); //!vsucnyt=1
  

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  TracyGpuContext;

  // configure imgui

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
  bool my_tool_active = true;

  bool i = ki.start();

  printf("start:%d\n", i);
  if (i != true) {
    return -1;
  }

  // osd.initializeOSD(); // could be wrapped into constructor!
  osdRenderer osd("extra_large");

  // render loop
  // -----------
  double curtime, lasttime = glfwGetTime();
  double frametime = 0;
  int frameslasts = 0;
  int fps = 0;

  // GAME LOOP
  TracyMessageL("Starting Game Loop");
  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    processInput(window);

    // ki.debugOsdPrint();

    waitpid(ki.pid, &ki.wstatus, WNOHANG);
    if (ki.wstatus != 0) {
      ki.isRunning = false;
    }

    // render
    // ------

    // ZoneScopedN("render");


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  

    

    osd.renderOSD(ki.shmem);
    
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse
    // moved etc.)
    // -------------------------------------------------------------------------------

    frametime = glfwGetTime() - curtime;
    curtime = glfwGetTime();
    if (curtime - lasttime >= 1) {
      // printf("%f ms/frame\n", 1000.0 / double(frameslasts));
      // printf("frames: %i\n", frameslasts);
      fps = frameslasts;
      frameslasts = 0;
      lasttime = curtime;
    } else {
      frameslasts += 1;
    }

    renderOverlay(ki);
    renderFPSbox(ki, frametime, fps);

    renderOSDOverlay(osd);
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    FrameMark;
    TracyGpuCollect;


    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------

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