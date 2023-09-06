//
// Created by Louis-Antoine Lebel on 2023-03-29.
//

#include "Molecule.h"
#include <iostream>

#include "glm/ext.hpp"
#include "../Dependencies/json.hpp"
#include <sstream>
#include <math.h>

// Colours for the atoms. The most common element in a molecule gets index 0, second-most gets index 1, and so on
std::vector<glm::vec3> mol_colours ={
        glm::vec3 (230.0,25.0,75.0),      // red
        glm::vec3 (70, 180.0, 75.0),      // Green
        glm::vec3 (0.0,130.0,200.0),      // Blue
        glm::vec3 (245.0, 130.0,48.0),    // Orange
        glm::vec3 (255.0, 225.0, 25.0),   // Yellow
        glm::vec3 (210.0,245.0, 60.0),    // Lime
        glm::vec3 (70.0,240.0,240.0),     // Cyan
        glm::vec3 (145.0, 30.0, 180.0),   // Purple
        glm::vec3 (140.0,50.0,230.0),     // Magenta
        glm::vec3 (128.0,0.0,0.0),        // Maroon
        glm::vec3 (170.0,170.0,40.0),     // Brown
        glm::vec3 (128.0,128.0,0.0),      // Olive
        glm::vec3 (0.0,128.0,128.0),      // Teal
        glm::vec3 (0.0,0.0,128.0),        // Navy
};


/* Implementation of string split functions that usually comes with any good programming language */
std::vector<std::string> split(std::string s, char delimeter=' '){

    std::vector<std::string> split_string; //string vector
    std::string current = ""; //current token


    for(int i = 0; i < s.size(); i++){
        if(s[i] == delimeter){ //split string
            if(current != ""){
                split_string.push_back(current);
                current = "";
            }
            continue;
        }

        current += s[i];
    }
    if(current.size() != 0)
        split_string.push_back(current);
    return split_string;
}

Molecule::Molecule(std::string path) {

    name = split(path.substr(17), '.')[0]; // get name of the molecule

    centre = glm::vec3 (0.0, 0.0, 0.0); // centre of the molecule

    std::vector<std::string> symbols; // vector of all the atomic symbols

    // open periodic table file
    std::ifstream t("../res/periodic_table.json");

    std::stringstream buffer;
    buffer << t.rdbuf();

    nlohmann::json j = nlohmann::json::parse(buffer.str());



    // read .mol file
    std::string line;
    std::ifstream mol (path);
    if (mol.is_open())
    {

        // Skip 3 first lines
        getline(mol, line);
        getline(mol, line);
        getline(mol, line);

        getline(mol, line);
        std::cout << "merp" << std::endl;
        std::cout << line.substr(0,3) << " " << line.substr(3,5) << std::endl;
        // int atom_num = std::stoi(split(line, ' ')[0]); //get number of atoms
        // int bond_num = std::stoi(split(line, ' ')[1]); //get number of bonds
        int atom_num = std::stoi(line.substr(0,3)); //get number of atoms
        int bond_num = std::stoi(line.substr(3,5)); //get number of bonds

        // Generate Atoms
        for(int i=0; i<atom_num; i++){
            getline(mol, line);


            std::string symbol = split(line, ' ')[3];

            // x, y, z coordinates of the atom
            float x = std::stof(split(line, ' ')[0]);
            float y = std::stof(split(line, ' ')[1]);
            float z = std::stof(split(line, ' ')[2]);


            glm::vec3 atom_pos = glm::vec3(x,y,z);

            // determine the radius of the atom from its atomic number
            float radius = 0.2 * log10(float(j[symbol]["number"])) + 0.2f;


            // Atom default colour (light grey)
            glm::vec3 colour = glm::vec3(0.8f, 0.8f, 0.8f);

            // create Atom object
            Atom* a = new Atom(radius, colour);
            a->translate(atom_pos);
            a->atomic_symbol = symbol;

            centre+=atom_pos;

            atoms.push_back(a);
            if(symbol!= "H")
                symbols.push_back(symbol);



        }

        centre = centre/atoms.size(); // get average position of all molecules (centre)

        // Generate Bonds
        for(int i=0; i<bond_num; i++){
            getline(mol, line);


            // index of the two atoms we're bonding
            // int a1 = std::stoi(split(line, ' ')[0]) -1;
            // int a2 = std::stoi(split(line, ' ')[1]) -1;

            int a1 = std::stoi(line.substr(0,3)) -1;
            int a2 = std::stoi(line.substr(3,5)) -1;

            // Single, double, or triple bond
            int bond_type = std::stoi(split(line, ' ')[2]);

            // Draw bond depending on its type
            if(bond_type == 1){

                Bond *b = new Bond(Type::Single);

                // move bond to the right location
                glm::vec3 bond_vector = glm::normalize(atoms[a2]->pos - atoms[a1]->pos);
                glm::vec3 translation = glm::vec3((atoms[a1]->pos[0] + atoms[a2]->pos[0]) / 2.0f,
                                                  (atoms[a1]->pos[1] + atoms[a2]->pos[1]) / 2.0f,
                                                  (atoms[a1]->pos[2] + atoms[a2]->pos[2]) / 2.0f);
                b->transform = glm::translate(b->transform, translation);
                auto rotation_y = acos(glm::dot(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b->transform = glm::translate(b->transform, -centre);
                b->transform = glm::rotate(b->transform, rotation_y, glm::cross(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b->transform = glm::scale(b->transform, glm::vec3(0.1, 0.1, 1.0));


                bonds.push_back(b);
            }
            else if(bond_type==2){ // Double bond
                Bond *b1 = new Bond(Type::Single);
                Bond *b2 = new Bond(Type::Single);

                glm::vec3 bond_vector = glm::normalize(atoms[a2]->pos - atoms[a1]->pos);


                glm::vec3 translation = glm::vec3((atoms[a1]->pos[0] + atoms[a2]->pos[0]) / 2.0f,
                                                  (atoms[a1]->pos[1] + atoms[a2]->pos[1]) / 2.0f,
                                                  (atoms[a1]->pos[2] + atoms[a2]->pos[2]) / 2.0f);

                auto rotation_y = acos(glm::dot(glm::vec3(0.0, 0.0, 1.0), bond_vector));

                b1->transform = glm::translate(b1->transform, translation);
                b1->transform = glm::translate(b1->transform, glm::vec3(0.1f, 0.0, 0.0));
                b1->transform = glm::translate(b1->transform, -centre);
                b1->transform = glm::rotate(b1->transform, rotation_y, glm::cross(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b1->transform = glm::scale(b1->transform, glm::vec3(0.05, 0.05, 1.0));

                b2->transform = glm::translate(b2->transform, translation);
                b2->transform = glm::translate(b2->transform, glm::vec3(-0.1f, 0.0, 0.0));
                b2->transform = glm::translate(b2->transform, -centre);
                b2->transform = glm::rotate(b2->transform, rotation_y, glm::cross(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b2->transform = glm::scale(b2->transform, glm::vec3(0.05, 0.05, 1.0));


                bonds.push_back(b1);
                bonds.push_back(b2);
            }
            else{ // Triple bond
                Bond *b1 = new Bond(Type::Single);
                Bond *b2 = new Bond(Type::Single);
                Bond *b3 = new Bond(Type::Single);

                glm::vec3 bond_vector = glm::normalize(atoms[a2]->pos - atoms[a1]->pos);


                glm::vec3 translation = glm::vec3((atoms[a1]->pos[0] + atoms[a2]->pos[0]) / 2.0f,
                                                  (atoms[a1]->pos[1] + atoms[a2]->pos[1]) / 2.0f,
                                                  (atoms[a1]->pos[2] + atoms[a2]->pos[2]) / 2.0f);

                auto rotation_y = acos(glm::dot(glm::vec3(0.0, 0.0, 1.0), bond_vector));

                b1->transform = glm::translate(b1->transform, translation);
                b1->transform = glm::translate(b1->transform, glm::vec3(0.1f, -0.1f, 0.0));
                b1->transform = glm::translate(b1->transform, -centre);
                b1->transform = glm::rotate(b1->transform, rotation_y, glm::cross(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b1->transform = glm::scale(b1->transform, glm::vec3(0.05, 0.05, 1.0));

                b2->transform = glm::translate(b2->transform, translation);
                b2->transform = glm::translate(b2->transform, glm::vec3(-0.1f, -0.1f, 0.0));
                b2->transform = glm::translate(b2->transform, -centre);
                b2->transform = glm::rotate(b2->transform, rotation_y, glm::cross(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b2->transform = glm::scale(b2->transform, glm::vec3(0.05, 0.05, 1.0));

                b3->transform = glm::translate(b3->transform, translation);
                b3->transform = glm::translate(b3->transform, glm::vec3(0.0f, 0.1f, 0.0));
                b3->transform = glm::translate(b3->transform, -centre);
                b3->transform = glm::rotate(b3->transform, rotation_y, glm::cross(glm::vec3(0.0, 0.0, 1.0), bond_vector));
                b3->transform = glm::scale(b3->transform, glm::vec3(0.05, 0.05, 1.0));

                bonds.push_back(b1);
                bonds.push_back(b2);
                bonds.push_back(b3);
            }
        }

    }

    mol.close(); // close file

    // Determine the colours
    std::map<std::string, int> atom_occurences;
    for (int i = 0; i < symbols.size(); i++) {
        atom_occurences[symbols[i]]++;
    }


    for (int i = 0; i < atom_occurences.size(); i++) {
        std::map<std::string, int>::iterator max
                = std::max_element(atom_occurences.begin(), atom_occurences.end(),
                                   [](const std::pair<std::string, int> &a,
                                      const std::pair<std::string, int> &b) -> bool { return a.second < b.second; });
        std::cout << max->first << " , " << max->second << "\n";
        atom_occurences[max->first] = i *-1;
    }

    // Some last-minute transforms
    for(Atom *a: atoms){
        // translate every atom by the negative of the atom centre so that the molecule is centered at 0
        a->transform = glm::translate(a->transform, -centre);

        if(a->atomic_symbol == "H") // Hydrogen atoms are always coloured the same
            continue;
        a->color = mol_colours[(atom_occurences[a->atomic_symbol]*-1) % mol_colours.size()]/255.0; // set colours
    }


}

Molecule::~Molecule() {}