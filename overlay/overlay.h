#pragma once

#include "../kernelinterface.hpp"
#include "../libraries/imgui/imgui.h"
#include "../libraries/imgui/imgui_impl_glfw.h"
#include "../libraries/imgui/imgui_impl_opengl3.h"
#include <csignal>

void renderOverlay(kernelInterface &ki);
void renderFPSbox(kernelInterface &ki, float frametime, int fps);
void showTexture(void *texture, int width, int height);