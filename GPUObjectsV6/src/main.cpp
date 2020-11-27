#include "pch.h"


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include "WorldScene.h"
#include "environment.h"



void printJob() {
    PROFILE_FUNCTION
    for (size_t i = 0; i < 100; i++)
    {
        std::cout << "job printing: " << i << std::endl;
    }
}

void transferJob(marl::Ticket ticket,int task) {
    ticket.wait();
    {
        PROFILE_FUNCTION
        for (size_t i = 0; i < 10; i++)
        {
            std::cout << "trasnfer  " << task << " printing: " << i << std::endl;
        }
    }
    ticket.done();
}


void jobTest() {

    marl::Scheduler scheduler(marl::Scheduler::Config::allCores());

    scheduler.bind(); 
    defer(scheduler.unbind());
    
    marl::Ticket::Queue queue;
    
    for (size_t i = 0; i < 10; i++)
        marl::schedule(transferJob, queue.take(), i);

    for (size_t i = 0; i < 10; i++)
        marl::schedule(printJob);

    for (size_t i = 0; i < 10; i++)
        marl::schedule(transferJob,queue.take(),i);
    queue.take().wait();
   /* for (size_t i = 0; i < 10; i++)*/
        printf("\n\n\nhello\n\n\n");

}





int main() {

  //  jobTest();
//    Instrumentor::Get().EndSession();
    //return 0;

    marl::Scheduler scheduler(marl::Scheduler::Config::allCores());

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