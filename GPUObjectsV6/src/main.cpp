#include "pch.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include "WorldScene.h"
#include "environment.h"





int main() {

  //  jobTest();
//    Instrumentor::Get().EndSession();
    //return 0;

    auto confic = marl::Scheduler::Config();

    confic.setWorkerThreadCount(std::thread::hardware_concurrency() - 1);

    marl::Scheduler scheduler(confic);

    scheduler.bind();
    defer(scheduler.unbind());

    Instrumentor::Get().BeginSession("Launch", "instruments_Launch.json");

    WindowManager* windowCreator = new WindowManager();
    
    windowCreator->camera.fov = 60;
    windowCreator->camera.zNear = 0.1;
    windowCreator->camera.zFar = 1'000'000;

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