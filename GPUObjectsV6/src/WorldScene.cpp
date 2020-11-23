#include "pch.h"
#include "WorldScene.h"
#include "CameraSystem.h"
#include "FloatingOriginSystem.h"

WorldScene::WorldScene(WindowManager& window) : window(window)
{
	PROFILE_FUNCTION
	renderer = new Renderer(window.device, window.physicalDevice, window);
	terrainSystem = new TerrainSystem(renderer,&origin);
	terrainSystem->trackedTransform = &playerTrans;
	terrainSystem->world = this;
	renderer->terrainSystem = terrainSystem;
	generalSystems = { new FloatingOriginSystem(), new CameraSystem() };

	for (System* sys : generalSystems) {
		sys->world = this;
	}
	generalSystems[0]->update();
}

WorldScene::~WorldScene()
{
	PROFILE_FUNCTION

	delete terrainSystem;
	delete renderer;

	for (System* sys : generalSystems) {
		delete sys;
	}

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

	window.device.waitIdle();
}

void WorldScene::runFullUpdateLoop()
{
	PROFILE_FUNCTION
	{
		PROFILE_SCOPE("glfwPollEvents");
		glfwPollEvents();
	}
	//drawView();

	// update scene
	updateScene();
	// draw view
	if (window.getDrawable() == true) return;
	renderer->renderFrame();
	window.presentDrawable();
}


void WorldScene::updateScene()
{
	PROFILE_FUNCTION

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	time = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - startTime).count();
	timef = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();





	for (System* sys : generalSystems) {
		sys->update();
	}

	terrainSystem->update();

	window.camera.transform = playerTrans;
}
