//
// Created by Louis-Antoine Lebel on 2023-04-01.
//

#ifndef OPENGLTEST_TBCAMERA_H
#define OPENGLTEST_TBCAMERA_H
#include <glm/glm.hpp>

class TBCamera {
public:
    TBCamera(float f, float ar, glm::vec3 init_pos);
    ~TBCamera();

    glm::mat4 world;
    glm::mat4 view;

    glm::mat4 getProjMatrix();
    void reset(glm::vec3 pos);

    float fov;
    float aspect_ratio;
};


#endif //OPENGLTEST_TBCAMERA_H
