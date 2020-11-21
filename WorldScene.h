#pragma once

#include "pch.h"
#include "ApplicationWindow.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include "TerrainSystem.h"
#include "environment.h"

class WorldScene
{

public:

	WorldScene(WindowManager& window);
	~WorldScene();

	WindowManager& window;
	
	Renderer* renderer;

	void startScene();

	void runFullUpdateLoop();

	TerrainSystem* terrainSystem;

	glm::dvec3 origin = glm::dvec3(0, 0, 0);

	// player

	glm::dvec3 playerLLA = glm::dvec3(0,0,10);
	Transform playerTrans;

private:

	void loadScene();

	void updateScene();

};

