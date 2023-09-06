//
// Created by Louis-Antoine Lebel on 2023-03-28.
//

#ifndef OPENGLTEST_ATOM_H
#define OPENGLTEST_ATOM_H
#include <vector>
#include <iostream>

#include <glm/ext/matrix_transform.hpp>

#include <GL/glew.h>

#include <glm/glm.hpp>


class Atom {
public:
    Atom(float radius, glm::vec3 colour);
    ~Atom();

    void translate(glm::vec3 t);
    void draw();

    GLuint VAO;
    GLuint EBO;
    std::size_t indexSize;

    glm::mat4 transform;
    glm::vec3 pos;

    glm::vec3 color;

    std::string atomic_symbol;
    float atomic_mass;
    int number;
};


#endif //OPENGLTEST_ATOM_H
