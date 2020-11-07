#include "WorldScene.h"

WorldScene::WorldScene(WindowManager& window) : window(window)
{
	renderer = new Renderer(window.device, window.physicalDevice, window);
}

WorldScene::~WorldScene()
{
	delete renderer;
}

void WorldScene::loadScene()
{

}

void WorldScene::startScene()
{
	while (!glfwWindowShouldClose(window.window)) {
		glfwPollEvents();
		//drawView();
		// draw view
		window.getDrawable();
		renderer->renderFrame();
		window.presentDrawable();
	}
}


void WorldScene::updateScene()
{
	renderer->renderFrame();
}
