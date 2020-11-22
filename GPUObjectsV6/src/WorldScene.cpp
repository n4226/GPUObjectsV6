#include "pch.h"
#include "WorldScene.h"

WorldScene::WorldScene(WindowManager& window) : window(window)
{
	PROFILE_FUNCTION
	renderer = new Renderer(window.device, window.physicalDevice, window);
	terrainSystem = new TerrainSystem(renderer);
	terrainSystem->trackedTransform = &playerTrans;
	terrainSystem->origin = &origin;
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

	// the camera looks at -> +z
	playerTrans.position = glm::vec3(0, 0,-4);
	//Math::LlatoGeo(playerLLA, glm::dvec3(0, 0, 10),1);
	window.camera.transform = playerTrans;

	terrainSystem->update();
}
