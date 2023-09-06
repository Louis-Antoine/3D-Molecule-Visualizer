//
// Created by Louis-Antoine Lebel on 2023-04-03.
//

#include "Skybox.h"
#include <vector>
#include <iostream>
#include "../Dependencies/stb_image.h"

Skybox::~Skybox() {}

Skybox::Skybox() {
    std::vector<glm::vec3> vertices =
            {
                    glm::vec3(-1.0f, -1.0f,  1.0f),
                    glm::vec3(1.0f, -1.0f,  1.0f),
                    glm::vec3(1.0f, -1.0f, -1.0f),
                    glm::vec3(-1.0f, -1.0f, -1.0f),
                    glm::vec3(-1.0f,  1.0f,  1.0f),
                    glm::vec3(1.0f,  1.0f,  1.0f),
                    glm::vec3(1.0f,  1.0f, -1.0f),
                    glm::vec3(-1.0f,  1.0f, -1.0f)
            };

    std::vector<GLuint> indices =
            {
                    // Right
                    1, 2, 6,
                    6, 5, 1,
                    // Left
                    0, 4, 7,
                    7, 3, 0,
                    // Top
                    4, 5, 6,
                    6, 7, 4,
                    // Bottom
                    0, 3, 2,
                    2, 1, 0,
                    // Back
                    0, 1, 5,
                    5, 4, 0,
                    // Front
                    3, 7, 6,
                    6, 2, 3
            };

    // Initialize VAO, VBO, EBO
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()* sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // There's only one image for the whole skybox
    std::string texture = "../res/bk.jpg";


    // Create the cubemap texture object
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(texture.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
                    (
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                            0,
                            GL_RGB,
                            width,
                            height,
                            0,
                            GL_RGB,
                            GL_UNSIGNED_BYTE,
                            data
                    );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
            stbi_image_free(data);
        }
    }

}

void Skybox::draw() {
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}