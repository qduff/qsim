#pragma once

#include "../engineinterface.hpp"
#include "../libraries/imgui/imgui.h"
#include "../renderers/osd.h"

#include <csignal>

void renderInterfaceOverlay(engineInterface &ki);
void renderFPSbox(engineInterface &ki, float frametime, int fps);
void showTexture(void *texture, int width, int height);
void renderOSDOverlay(osdRenderer &osd);