#include "osd.h"
#include "Tracy.hpp"
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

osdRenderer::~osdRenderer() {
  glDeleteVertexArrays(1, &VAO_osd);
  glDeleteBuffers(1, &VBO_osd);
  glDeleteBuffers(1, &EBO_osd);
  glDeleteTextures(1, &OSDTEX_osd);
}

osdRenderer::osdRenderer(std::string fontname) {

  glGenVertexArrays(1, &VAO_osd);
  glGenBuffers(1, &VBO_osd);
  glGenBuffers(1, &EBO_osd);

  glBindVertexArray(VAO_osd);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_osd);
  glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_osd);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (GLvoid *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenTextures(1, &OSDTEX_osd);

  loadTex(fontname);
};

void osdRenderer::loadTex(std::string fontName) {
  glBindTexture(GL_TEXTURE_2D, OSDTEX_osd);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height;
  auto *data = loadMCM(("../resources/osdfonts/" + fontName + ".mcm").c_str(),
                       width, height);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  // printf("%llu\n\n", sizeof(data));
  // Don't forget to free memory!
}

std::vector<std::string> osdRenderer::getOSDFonts() {
  return std::vector<std::string>{"default", "impact", "extra_large"};
};

void osdRenderer::changeOSDFont(std::string fname) {
  loadTex(fname);
  //! CHANGE TEX
}

void osdRenderer::renderOSD(memory_s *shmem, int width, int height) {
  renderOSDStupidly(shmem, width, height);
}

float tex_xl, tex_xr, ryu, ryd, rxl, rxr; // removeme

void osdRenderer::renderOSDStupidly(memory_s *shmem, int width, int height) {
  ZoneScoped;
  // height = 288, width = 360
  float scale = ((float)height / (float)width) / (288.0f / 360.0f);
  float wscale = 1;
  float hscale = 1;
  if (scale < 1) {
    wscale = scale;
  } else {
    hscale = 1 / scale;
  }

  {
    ZoneScopedN("Initialize");
    glBindVertexArray(VAO_osd);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_osd);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, OSDTEX_osd);
    osdShader.use();
  }

  // osdShader.setInt("ourTexture", 0);

  {
    const int width = 30;
    const int height = 16;
    for (char y = 0; y < 16; ++y) { //
      for (char x = 0; x < 30; ++x) {
        unsigned char curchar = shmem->osd[(15 - y) * width + x];
        // curchar = "DEBUG"[(y * width + x) % 5];
        if (curchar == 0 || curchar == 32) { // no need to render if 0 or null!
          continue;
        }
        ZoneScopedN("inloop draw");
        ZoneValue(curchar);
        // printf("x%iy%i - %c \n", x, y, curchar);
        //

        // SELECTS THE CHARACTER WE WANT
        tex_xl = curchar / 256.0f;
        tex_xr = (curchar + 1) / 256.0f;

        // SELECTS THE RENDERING LOCATION
        ryu = (-1 + 2 * ((float)(y) / height)) * hscale;
        ryd = (-1 + 2 * ((float)(y + 1) / height)) * hscale;
        rxl = (-1 + 2 * ((float)(x) / width)) * wscale;
        rxr = (-1 + 2 * ((float)(x + 1) / width)) * wscale;
        // printf("y:%f -> %f", ryd, ryu);
        //* it is important to note that these have to be rendered
        //* flipped in the Y-AXIS
        float vertices[] = {

            // apos(2), texcoord(2)
            rxr, ryu, tex_xr, 1.0f, // top right
            rxr, ryd, tex_xr, 0.0f, // bottom right
            rxl, ryd, tex_xl, 0.0f, // bottom left
            rxl, ryu,  tex_xl, 1.0f, // top left
        };
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
        //  GL_DYNAMIC_DRAW);
        {
          ZoneScopedN("glBufferSubData");
          glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        }
        {
          ZoneScopedN("glDrawElements");
          glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
      }
    }
  }
}
