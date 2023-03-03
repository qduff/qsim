#include <fstream>
#include <iostream>
#include <string>

#define charWidth 12
#define charHeight 18

#define numChars 256

int texwidth = charWidth * numChars;

#define bitdepth 4

struct color_s {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

struct color_s black = color_s{0b00000000, 0b00000000, 0b00000000, 0b11111111};
struct color_s white = color_s{0b11111111, 0b11111111, 0b11111111, 0b11111111};
struct color_s transp = color_s{0b11111111, 0b11111111, 0b11111111, 0b00000000};

// Converts a MAX7456 MCM file to a array that can be used by OpenGL
// You should free the image memory after loading it into the GPU!
unsigned char *loadMCM(char const *fpath, int &ret_texwidth,
                       int &ret_texheight) {

  static unsigned char *image =
      new unsigned char[charWidth * charHeight * numChars * bitdepth];

  std::ifstream is(fpath);
  std::string byte;
  std::string crumb;

  struct color_s color {};

  int counter = -1;

  int pixacross = 0;
  int pixdown = 0;

  while (getline(is, byte)) {
    if (!(counter == -1 || (counter % 64 > 53))) {
      for (int pos = 0; pos < 8; pos += 2) {
        crumb = byte.substr(pos, 2);
        if (crumb == "00")
          color = black;
        if (crumb == "01")
          color = transp;
        if (crumb == "10")
          color = white;
        if (crumb == "11")
          color = transp;

        image[(texwidth * pixdown + pixacross) * bitdepth] = color.r;
        image[(texwidth * pixdown + pixacross) * bitdepth + 1] = color.g;
        image[(texwidth * pixdown + pixacross) * bitdepth + 2] = color.b;
        image[(texwidth * pixdown + pixacross) * bitdepth + 3] = color.a;
        pixacross += 1;
      }
      if (pixacross % charWidth == 0) {
        pixacross -= charWidth;
        pixdown += 1;
      }
      if (pixdown == charHeight) {
        pixdown = 0;
        pixacross += charWidth;
      }
    }
    ++counter;
  }
  ret_texwidth = texwidth;
  ret_texheight = charHeight;
  return image;
};

unsigned char *testLoader(char const *name, int &texwidth, int &texheight) {
  int width = 256;
  int height = 256;
  unsigned char *image = new unsigned char[width * height * bitdepth];

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (x - y < 0) {
        image[(y * width + x) * bitdepth] = 0b11111111;
        image[(y * width + x) * bitdepth + 1] = 0b11111111;
        image[(y * width + x) * bitdepth + 2] = 0b11111111;
        image[(y * width + x) * bitdepth + 3] = 0b11111111;
      }
    }
  }
  texwidth = width;
  texheight = height;
  return image;
};