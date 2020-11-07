#pragma once

#include <vulkan/vulkan.hpp>
#include "ApplicationWindow.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>

class WorldScene
{

public:

	WorldScene(WindowManager& window);
	~WorldScene();

	WindowManager& window;
	
	Renderer* renderer;

	void startScene();

private:

	void loadScene();

	void updateScene();

};

