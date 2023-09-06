//
// Created by Louis-Antoine Lebel on 2023-04-01.
//

#include "TBCamera.h"
#include <glm/glm.hpp>
#include "glm/ext.hpp"

TBCamera::TBCamera(float f, float ar, glm::vec3 init_pos) {
    fov = 90;
    aspect_ratio = ar;

    world = glm::mat4 (1.0f);
    view = glm::translate(glm::mat4(1.0f), init_pos);
    view = glm::translate(view, glm::vec3(0.0, 0.0, -10));
}

TBCamera::~TBCamera() {}

void TBCamera::reset(glm::vec3 pos) {
    fov = 90;
    world = glm::mat4 (1.0f);

    view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -10));
    // view = glm::translate(view, pos);
}

glm::mat4 TBCamera::getProjMatrix() {
    return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
}