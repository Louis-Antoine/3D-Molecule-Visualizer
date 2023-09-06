//
// Created by Louis-Antoine Lebel on 2023-03-29.
//

#ifndef OPENGLTEST_MOLECULE_H
#define OPENGLTEST_MOLECULE_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fstream>
#include "Atom.h"
#include "Bond.h"

class Molecule {
public:
    Molecule(std::string path);
    ~Molecule();

    std::vector<Atom*> atoms;
    std::vector<Bond*> bonds;

    glm::vec3 centre;
    std::string name;
};


#endif //OPENGLTEST_MOLECULE_H
