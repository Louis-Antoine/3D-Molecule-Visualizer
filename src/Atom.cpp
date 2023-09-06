//
// Created by Louis-Antoine Lebel on 2023-03-28.
//

#include "Atom.h"
#include <iostream>

Atom::Atom(float radius, glm::vec3 colour) {
    transform = glm::mat4(1.0f);
    color = colour;

    // generate vertices, normals and indices for a sphere
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;

    float x;
    float y;
    float z;

    float r =radius;

    float n_x;
    float n_y;
    float n_z;

    // default sector and stack count
    int sector_num = 30;
    int stack_num = 12;

    float sector_angle;
    float stack_angle;

    float sector_step = 2 * (M_PI / sector_num);
    float stack_step = (M_PI / stack_num);


    // Generate vertices
    for(int i=0; i<= stack_num; i++){
        stack_angle = (M_PI / 2) - i * stack_step;
        z = r * sinf(stack_angle);

        for(int j =0; j<=sector_num; j++){
            sector_angle = j*sector_step;

            x = r*cosf(stack_angle) * cosf(sector_angle);
            y = r*cosf(stack_angle) * sinf(sector_angle);

            vertices.push_back(glm::vec3(x,y,z));

            n_x = x* (1/r);
            n_y = y* (1/r);
            n_z = z* (1/r);

            normals.push_back(glm::vec3(n_x,n_y,n_z));

        }
    }

    int k1;
    int k2;

    // generate indices
    for(int i=0; i<stack_num; i++){
        k1 = i*(sector_num+1);
        k2 = k1 + sector_num +1;

        for(int j=0; j<sector_num; j++, ++k1, ++k2){
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stack_num-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // Initialize VAO, VBO, NBO, EBO
    GLuint VBO;
    GLuint NBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    indexSize = indices.size();

}

Atom::~Atom() {}

void Atom::draw() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Atom::translate(glm::vec3 t) {
    pos = t;
    transform = glm::translate(transform, t);
}

