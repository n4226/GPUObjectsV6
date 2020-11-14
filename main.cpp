
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "WorldScene.h"

int main() {

    WindowManager* windowCreator = new WindowManager();
    
    windowCreator->camera.fov = 60;
    windowCreator->camera.zNear = 0.01;
    windowCreator->camera.zFar = 1000;

    WorldScene* scene = new WorldScene(*windowCreator);

    printf("starting render loop");

    scene->startScene();

    delete scene;
    delete windowCreator;

    return 0;
}