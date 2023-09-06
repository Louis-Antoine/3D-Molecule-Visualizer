//
// Created by Louis-Antoine Lebel on 2023-03-29.
//

#ifndef OPENGLTEST_BOND_H
#define OPENGLTEST_BOND_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>

enum class Type{
    Single,
    Double,
    Triple
};

class Bond {
public:


    Bond(Type t);
    ~Bond();

    void draw();

    GLuint VAO;
    GLuint EBO;
    std::size_t indexSize;

    glm::mat4 transform;

    glm::vec3 color;

    Type type;


};


#endif //OPENGLTEST_BOND_H
