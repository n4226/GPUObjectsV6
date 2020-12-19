#include "pch.h"
#include "Application/ApplicationRendererBridge/Application.h"




int main() {
    {
        auto app = new Application();

        printf("starting render loop\n");

        app->run();
    }
    return 0;
}

