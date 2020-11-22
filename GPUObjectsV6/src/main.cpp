#include "pch.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "WorldScene.h"
#include "environment.h"

int main() {

    Instrumentor::Get().BeginSession("Launch", "instruments_Launch.json");

    WindowManager* windowCreator = new WindowManager();
    
    windowCreator->camera.fov = 60;
    windowCreator->camera.zNear = 0.01;
    windowCreator->camera.zFar = 1000;

    WorldScene* scene = new WorldScene(*windowCreator);
    
    Instrumentor::Get().EndSession();
    Instrumentor::Get().BeginSession("Run", "instruments_Run.json");

    printf("starting render loop\n");

    scene->startScene();

    Instrumentor::Get().EndSession();
    Instrumentor::Get().BeginSession("Shutdown", "instruments_Shutdown.json");

    delete scene;
    delete windowCreator;

    Instrumentor::Get().EndSession();

    return 0;
}