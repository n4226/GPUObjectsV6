#include "pch.h"
#include "WorldScene.h"
#include "CameraSystem.h"
#include "FloatingOriginSystem.h"

WorldScene::WorldScene(WindowManager& window) : 
	window(window)
{
	PROFILE_FUNCTION
	renderer = new Renderer(window.device, window.physicalDevice, window);
	renderer->world = this;
	terrainSystem = new TerrainSystem(renderer,&origin);
	terrainSystem->trackedTransform = &playerTrans;
	terrainSystem->world = this;
	renderer->terrainSystem = terrainSystem;
	generalSystems = { new FloatingOriginSystem(), new CameraSystem() };

	for (System* sys : generalSystems) {
		sys->world = this;
	}

	time = 0;
	timef = 0;

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
	static auto startTime = std::chrono::high_resolution_clock::now();
	PROFILE_FUNCTION


	auto currentTime = std::chrono::high_resolution_clock::now();
	auto newtime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - startTime).count();
	auto newtimef = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	deltaTime = newtime - time;
	deltaTimef = newtimef - timef;

	time = newtime;
	timef = newtimef;

	if (frameNum == 2000) {
		char** stats = new char*;
		vmaBuildStatsString(renderer->allocator, stats,VK_TRUE);
		//std::string statString(*stats);

		{
			std::ofstream out;
			out.open("vmaDump.profile", std::fstream::out);
			//out.open(file, std::fstream::out);
			out << *stats;
			out.close();
		}
		vmaFreeStatsString(renderer->allocator, *stats);
	}

	for (System* sys : generalSystems) {
		sys->update();
	}

	terrainSystem->update();

	window.camera.transform = playerTrans;

	frameNum += 1;
}
