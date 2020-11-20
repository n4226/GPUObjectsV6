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

private:

	void loadScene();

	void updateScene();

};

