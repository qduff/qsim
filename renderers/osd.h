
#include "../memdef.h"
#include "../shader.hpp"
#include "TracyOpenGL.hpp"
#include <glad/glad.h>


void renderOSD(Shader &s, memory_s *shmem, unsigned int texture,
               unsigned int VAO, unsigned int VBO) {
  TracyGpuZone("OSD Render");
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindTexture(GL_TEXTURE_2D, texture);
  s.use();
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
