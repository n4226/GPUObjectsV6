#pragma once

#include "pch.h"


struct TriangleUniformBufferObject {
    glm::mat4 model;
    glm::mat4 viewProjection;
};

struct SceneUniforms {
    glm::mat4 viewProjection;
};

struct PostProcessEarthDatAndUniforms {
    glm::mat4 invertedViewMat;
    glm::mat4 viewMat;
    glm::mat4 projMat;

    glm::vec4 earthCenter;
    glm::vec4 sunDir;
    glm::vec4 camFloatedGloabelPos;
    glm::ivec2 renderTargetSize;
};

struct ModelUniforms {
    glm::mat4 model;
};

struct DrawPushData
{
    glm::uint32 modelIndex;
};
