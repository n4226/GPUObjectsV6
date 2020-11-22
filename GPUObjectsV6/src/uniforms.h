#pragma once

#include <glm/glm.hpp>

struct TriangleUniformBufferObject {
    glm::mat4 model;
    glm::mat4 viewProjection;
};
