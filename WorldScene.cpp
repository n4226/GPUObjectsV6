#include "WorldScene.h"

WorldScene::WorldScene(WindowManager& window) : window(window)
{
	PROFILE_FUNCTION
	renderer = new Renderer(window.device, window.physicalDevice, window);
	terrainSystem = new TerrainSystem();
	renderer->terrainSystem = terrainSystem;
}

WorldScene::~WorldScene()
{
	PROFILE_FUNCTION
	delete renderer;
	delete terrainSystem;
}

void WorldScene::loadScene()
{
	PROFILE_FUNCTION
}

void WorldScene::startScene()
{
	loadScene();
	while (!glfwWindowShouldClose(window.window)) {
		runFullUpdateLoop();
	}
}

void WorldScene::runFullUpdateLoop()
{
	PROFILE_FUNCTION
	glfwPollEvents();
	//drawView();
	// update scene
	updateScene();
	// draw view
	window.getDrawable();
	renderer->renderFrame();
	window.presentDrawable();
}


void WorldScene::updateScene()
{
	PROFILE_FUNCTION
	terrainSystem->update();
}
