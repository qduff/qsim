// clang-format off



#ifdef __linux__
#include <sys/wait.h>
#include <csignal>
#include <unistd.h>
#elif __MINGW32__
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

#include "kernelinterface.hpp"
#include "shader.hpp"

#include "renderers/osd.h"
#include "renderers/scene.h"


#include "logger/logger.hpp"

#include <iostream>

// clang-format on

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
  kernelInterface ki;


int main() {



  Logger& logger = Logger::getLogger();
  logger.prioritylevel = 0;
  logger.Log(Logger::INFO,  "Starting");
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
  glfwSwapInterval(0); //! vsucnyt=1

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
  

  sceneRenderer scene;

  // render loop
  // -----------
  double curtime, lasttime = glfwGetTime();
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
    #ifdef __linux__  // !win32 alt?
    waitpid(ki.pid, &ki.wstatus, WNOHANG);
    if (ki.wstatus != 0) {
      ki.isRunning = false;
    }
    #endif
       //!todo  WaitForSingleObject( pi.hProcess, INFINITE ); windows equivalent!
       //* thus store this in the ki class, actuially move this entire block terheh


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

    scene.render(SCR_WIDTH, SCR_HEIGHT);
    osd.renderOSD(ki.shmem, SCR_WIDTH, SCR_HEIGHT);

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
      renderOverlay(ki);
      renderFPSbox(ki, frametime, fps);
      renderOSDOverlay(osd);
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
    // glfwGetJoystickName(GLFW_JOYSTICK_1);
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
  SCR_WIDTH = width;
  SCR_HEIGHT = height;
  glViewport(0, 0, width, height);
}