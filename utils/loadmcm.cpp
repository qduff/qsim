#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#define CHAR_WIDTH 12
#define CHAR_HEIGHT 18

#define NUM_CHARS 256

// #define texwidth = CHAR_WIDTH * NUM_CHARS;

#define BITDEPTH 4

struct color_s {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct color_s black = color_s{0b00000000, 0b00000000, 0b00000000, 0b11111111};
struct color_s white = color_s{0b11111111, 0b11111111, 0b11111111, 0b11111111};
struct color_s transp = color_s{0b11111111, 0b11111111, 0b11111111, 0b00000000};

// Converts a MAX7456 MCM file to a array that can be used by OpenGL
// You should free the image memory after loading it into the GPU!
auto *loadMCM(char const *fpath, int &ret_texwidth, int &ret_texheight) {
  auto *image = new uint8_t[CHAR_HEIGHT][CHAR_WIDTH  * NUM_CHARS][BITDEPTH];
  std::ifstream is(fpath);
  std::string byte;
  std::string crumb;

  struct color_s color {};

  int counter = 0;

  int pixacross = 0;
  int pixdown = 0;

  getline(is, byte); // MAX7456 - can be ignored
  while (getline(is, byte)) {
    if (!(counter % 64 > 53)) {
      for (int pos = 0; pos < 8; pos += 2) {
        crumb = byte.substr(pos, 2);
        if (crumb == "00")
          color = black;
        else if (crumb == "01")
          color = transp;
        else if (crumb == "10")
          color = white;
        else if (crumb == "11")
          color = transp;

        image[pixdown][pixacross][0] = color.r;
        image[pixdown][pixacross][1] = color.g;
        image[pixdown][pixacross][2] = color.b;
        image[pixdown][pixacross][3] = color.a;

        pixacross += 1;
      }
      if (pixacross % CHAR_WIDTH == 0) {
        pixacross -= CHAR_WIDTH;
        if (pixdown == CHAR_HEIGHT - 1) {
          pixdown = 0;
          pixacross += CHAR_WIDTH;
        } else {
          pixdown += 1;
        }
      }
    }
    ++counter;
  }
  ret_texwidth = CHAR_WIDTH * NUM_CHARS;
  ret_texheight = CHAR_HEIGHT;
  return image;
};

uint8_t *testLoader(char const *name, int &texwidth, int &texheight) {
  int width = 256;
  int height = 256;
  uint8_t *image = new uint8_t[width * height * BITDEPTH];

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (x - y < 0) {
        image[(y * width + x) * BITDEPTH] = 0b11111111;
        image[(y * width + x) * BITDEPTH + 1] = 0b11111111;
        image[(y * width + x) * BITDEPTH + 2] = 0b11111111;
        image[(y * width + x) * BITDEPTH + 3] = 0b11111111;
      }
    }
  }
  texwidth = width;
  texheight = height;
  return image;
};