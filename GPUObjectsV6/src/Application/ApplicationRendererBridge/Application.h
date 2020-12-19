#pragma once

#include "pch.h"
#include "RenderEngine/systems/renderSystems/Renderer.h"

class Application
{
public:

	Application();
	~Application();

	void run();

private:
	void startup();

	WorldScene* worldScene;


	WindowManager* windows;

	vk::Device* devices;
	Renderer* renderers;


	marl::Scheduler* scheduler;

};

