#pragma once

#include "pch.h"


struct TriangleUniformBufferObject {
    glm::mat4 model;
    glm::mat4 viewProjection;
};

struct SceneUniforms {
    glm::mat4 viewProjection;
};

struct ModelUniforms {
    glm::mat4 model;
};

struct DrawPushData
{
    glm::uint32 modelIndex;
};
