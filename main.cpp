// clang-format off



#ifdef __linux__
#include <sys/wait.h>
#include <csignal>
#include <unistd.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#include "TracyOpenGL.hpp"
#endif


#include "libraries/imgui/imgui.h"
#include "libraries/imgui/imgui_impl_glfw.h"
#include "libraries/imgui/imgui_impl_opengl3.h"

#include "overlay/overlay.h"

#include "memdef.h"
#include "preferences.h"

#include "engineinterface.hpp"
#include "shader.hpp"

#include "renderers/osd.h"
#include "renderers/scene.h"


#include "logger/logger.hpp"

#include "input/input.hpp"

#include <iostream>

// clang-format on

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
engineInterface engine;
Input rcinput;

int main() {

  Logger &logger = Logger::getLogger();
  // Logger logger;
  logger.prioritylevel = 0;
  logger.Log(Logger::INFO, "Starting");
  logger.Log(Logger::INFO, "Width = %d, Height = %d\n", SCR_WIDTH, SCR_HEIGHT);

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
  glfwSwapInterval(1); //! vsucnyt=1, unlimit=0

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


  if (!engine.start()) {
    puts("could not start engine, quitting!");
    return -1;
  }

  osdRenderer osd("extra_large");
  sceneRenderer scene;

  // render loop
  // -----------
  double curtime = glfwGetTime(), lasttime = glfwGetTime();
  double frametime = 0;
  int frameslasts = 0;
  int fps = 0;

  glEnable(GL_DEPTH_TEST);

  // GAME LOOP
  TracyMessageL("Starting Game Loop");
  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    processInput(window);

// ki.debugOsdPrint();
    engine.checkRunning();
    // render
    // ------

    ZoneScopedN("render");
    TracyGpuZone("main?")

    {
      ZoneScopedN("Imguinewframe");
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }

    {
      ZoneScopedN("Clear");
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    scene.render(engine.shmem, SCR_WIDTH, SCR_HEIGHT);
    osd.renderOSD(engine.shmem, SCR_WIDTH, SCR_HEIGHT);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse
    // moved etc.)
    // -------------------------------------------------------------------------------

    {
      ZoneScopedN("calctimes");
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
    }
    {
      // ZoneScopedN("IMGUI Overlay:");
      renderInterfaceOverlay(engine);
      renderFPSbox(engine, frametime, fps);
      renderOSDOverlay(osd);
      rcinput.renderOverlay();
      {
        ZoneScopedN("RENDER OVERLAY");

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      }
    }

    {
      ZoneScopedN("swapbuffer");
      glfwSwapBuffers(window);
    }

    FrameMark;
    TracyGpuCollect;

    {
      ZoneScopedN("polling");
      glfwPollEvents();
    }
  }
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
void processInput(GLFWwindow *window) { //! move this callback to input.cpp
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  rcinput.present = glfwJoystickPresent(GLFW_JOYSTICK_1);
  if (rcinput.present) {
    rcinput.axesvalues = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &rcinput.axescount);
    rcinput.buttonvalues = glfwGetJoystickButtons(GLFW_JOYSTICK_3, &rcinput.buttoncount);
    rcinput.name = glfwGetJoystickName(GLFW_JOYSTICK_1);
    rcinput.processAxes(engine);
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  SCR_WIDTH = width;
  SCR_HEIGHT = height;
  glViewport(0, 0, width, height);
}
