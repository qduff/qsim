#include "osd.h"
#include <filesystem>
#include <string>
#include <vector>

osdRenderer::~osdRenderer() {
  // glDeleteVertexArrays(1, &VAO_osd);
  // glDeleteBuffers(1, &VBO_osd);
  // glDeleteBuffers(1, &EBO_osd);
}

osdRenderer::osdRenderer(std::string fontname) {

  glGenVertexArrays(1, &VAO_osd);
  glGenBuffers(1, &VBO_osd);
  glGenBuffers(1, &EBO_osd);

  glBindVertexArray(VAO_osd);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_osd);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float), NULL, GL_DYNAMIC_DRAW);
  unsigned int indices[] = {0, 1, 3, 1, 2, 3};
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_osd);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (GLvoid *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glGenTextures(1, &TEX_osd);

 loadTex(fontname);
};

void osdRenderer::loadTex(std::string fontName){

  glBindTexture(GL_TEXTURE_2D, TEX_osd);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height;

  printf("loading %s\n",fontName.c_str());
  void *data =
      loadMCM(("../resources/osdfonts/"+fontName+".mcm").c_str(), width, height);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  // delete data; // ! THIS WILL ALMOST CERTAINLY NEVER WORK 
}



std::vector<std::string> osdRenderer::getOSDFonts(){
    return std::vector<std::string>{"default","impact","extra_large"};
};

void osdRenderer::changeOSDFont(std::string fname) {
  loadTex(fname);
  //! CHANGE TEX
}

void osdRenderer::renderOSD(memory_s *shmem) {
  TracyGpuZone("OSD Render");
  glBindBuffer(GL_ARRAY_BUFFER, VBO_osd);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, TEX_osd);
  osdShader.use();
  osdShader.setInt("ourTexture", 0);
  const int width = 30;
  const int height = 16;
  for (char y = 0; y < 16; ++y) { //
    for (char x = 0; x < 30; ++x) {
      unsigned char curchar = shmem->osd[(15 - y) * width + x];
      // if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
      //   curchar = 222; // asterisk
      // }
      // curchar = y * width + x;
      // printf("x%iy%i - %c \n", x, y, curchar);
      #ifdef dbgonosd
      curchar = "DEBUG"[(y * width + x)%5];
      #endif

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
      glBindVertexArray(VAO_osd);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
  }
}
