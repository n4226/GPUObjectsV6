#include "pch.h"
#include "Application.h"
#include "WorldScene.h"

Application::Application()
{
    startup();
}

Application::~Application()
{
    PROFILE_FUNCTION;
    Instrumentor::Get().BeginSession("Shutdown", "instruments_Shutdown.profile");


    delete worldScene;
    //Remember to delete all of theme
    delete windows;
    //delete renderers;

    scheduler->unbind();

    Instrumentor::Get().EndSession();
}

void Application::startup()
{
    PROFILE_FUNCTION;
    Instrumentor::Get().BeginSession("Launch", "instruments_Launch.profile");


    {// Configure Marl
        auto confic = marl::Scheduler::Config();

        confic.setWorkerThreadCount(std::thread::hardware_concurrency() - 1);

        scheduler = new marl::Scheduler(confic);

        scheduler->bind();
    }

    configSystem.readFromDisk();

    windows = new WindowManager();

    windows->camera.fov = 60;
    windows->camera.zNear = 0.1;
    windows->camera.zFar = 1'000'000;

    worldScene = new WorldScene(*windows);


    Instrumentor::Get().EndSession();
}

void Application::run()
{
    PROFILE_FUNCTION;
    Instrumentor::Get().BeginSession("Run", "instruments_Run.profile");

    worldScene->startScene();

    windows->device.waitIdle();

    Instrumentor::Get().EndSession();
}
