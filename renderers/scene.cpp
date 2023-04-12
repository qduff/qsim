// #include "Tracy.hpp"
#include "scene.h"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


sceneRenderer::sceneRenderer() {
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //
      0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, //
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //
      -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
      0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, //
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, //
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f  //
  };
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // pos attr
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);



  // texturing
  //  tex1
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("../resources/textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);// not necessary>??

    // tex2

        glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("../resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    cubeShader.use();
    cubeShader.setInt("texture1", 0);
    cubeShader.setInt("texture2", 1);




};

sceneRenderer::~sceneRenderer() {
  // delete stuff here


}

void sceneRenderer::render(int width, int height) {
  // puts("renderscene!");

    glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);

  cubeShader.use();

      // create transformations
    glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    // retrieve the matrix uniform locations
    unsigned int modelLoc = glGetUniformLocation(cubeShader.ID, "model");
    unsigned int viewLoc  = glGetUniformLocation(cubeShader.ID, "view");
    // pass them to the shaders (3 different ways)
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    cubeShader.setMat4("projection", projection);

    // render box
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}
