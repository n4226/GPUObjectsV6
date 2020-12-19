#include "pch.h"
#include "WorldScene.h"
#include "../../RenderEngine/systems/CameraSystem.h"
#include "../../RenderEngine/systems/FloatingOriginSystem.h"
#include "Application/ApplicationRendererBridge/Application.h"

WorldScene::WorldScene(Application& app) : 
	app(app)
{
	PROFILE_FUNCTION
		
}

void WorldScene::load()
{
	PROFILE_FUNCTION;
	//renderer = new Renderer(window.device, window.physicalDevice, window);
	//renderer->world = this;
	terrainSystem = new TerrainSystem(app, *this, &origin);
	terrainSystem->trackedTransform = &playerTrans;
	terrainSystem->world = this;

	for (auto render : app.renderers)
		render->terrainSystem = terrainSystem;

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

	for (System* sys : generalSystems) {
		delete sys;
	}

}

void WorldScene::loadScene()
{
	PROFILE_FUNCTION;




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
		vmaBuildStatsString(app.renderers[0]->allocator, stats,VK_TRUE);
		//std::string statString(*stats);

		{
			std::ofstream out;
			out.open("vmaDump.profile", std::fstream::out);
			//out.open(file, std::fstream::out);
			out << *stats;
			out.close();
		}
		vmaFreeStatsString(app.renderers[0]->allocator, *stats);
	}

	for (System* sys : generalSystems) {
		sys->update();
	}

	terrainSystem->update();

	for (auto window : app.windows)
		window->camera.transform = playerTrans;

	frameNum += 1;
}
