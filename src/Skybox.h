//
// Created by Louis-Antoine Lebel on 2023-04-03.
//

#ifndef OPENGLTEST_SKYBOX_H
#define OPENGLTEST_SKYBOX_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Skybox {
public:
    Skybox();
    ~Skybox();

    void draw();

    GLuint VAO;
    GLuint EBO;
    unsigned int cubemapTexture;
};


#endif //OPENGLTEST_SKYBOX_H
