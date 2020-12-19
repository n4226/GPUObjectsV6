#pragma once

#include "pch.h"
#include "ApplicationWindow.h"
#include "../../RenderEngine/systems/renderSystems/Renderer.h"
#include <GLFW/glfw3.h>
#include "../../RenderEngine/systems/terrainSystem/TerrainSystem.h"
#include "environment.h"

class WorldScene
{

public:

	WorldScene(Application& app);
	~WorldScene();


	Application& app;

	void load();
	void runFullUpdateLoop();

	TerrainSystem* terrainSystem;

	std::vector<System*> generalSystems;

	glm::dvec3 origin = glm::dvec3(0, 0, 0);

	// player

	glm::dvec3 playerLLA = 
		//glm::dvec3(0, 0, 1000);
		glm::dvec3(40.610319941413, -74.039182662964, 100);
	Transform playerTrans;

	double time = 0;
	float timef = 0;

	double deltaTime = 0;
	float  deltaTimef = 0;

	size_t frameNum = 0;

private:

	void loadScene();

	void updateScene();
	friend Application;
};

