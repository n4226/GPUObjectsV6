#include "pch.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include "WorldScene.h"
#include "environment.h"
#include "main.h"




int main() {

    Instrumentor::Get().BeginSession("Launch", "instruments_Launch.profile");

    // Configure Marl
    auto confic = marl::Scheduler::Config();

    confic.setWorkerThreadCount(std::thread::hardware_concurrency() - 1);

    marl::Scheduler scheduler(confic);

    scheduler.bind();
    defer(scheduler.unbind());

    configSystem.readFromDisk();

    WindowManager* windowCreator = new WindowManager();
    
    windowCreator->camera.fov = 60;
    windowCreator->camera.zNear = 0.1;
    windowCreator->camera.zFar = 1'000'000;

    WorldScene* scene = new WorldScene(*windowCreator);
    
    Instrumentor::Get().EndSession();
    Instrumentor::Get().BeginSession("Run", "instruments_Run.profile");

    printf("starting render loop\n");

    scene->startScene();

    Instrumentor::Get().EndSession();
    Instrumentor::Get().BeginSession("Shutdown", "instruments_Shutdown.profile");

    delete scene;
    delete windowCreator;

    Instrumentor::Get().EndSession();

    return 0;
}

