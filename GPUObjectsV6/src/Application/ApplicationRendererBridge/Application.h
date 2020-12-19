#pragma once

#include "pch.h"
#include "RenderEngine/systems/renderSystems/Renderer.h"

class Application
{
public:

	Application();
	~Application();

	void run();
	const size_t MAX_FRAMES_IN_FLIGHT = 3;
	size_t maxSwapChainImages = 0;

	// holding objects

	WorldScene* worldScene;
	marl::Scheduler* scheduler;

	// vulkan and render objects

	vk::Instance instance;


	std::vector<WindowManager*> windows;

	// - one per each device - shared indicies
	std::vector<vk::Device         >          devices;
	std::vector<vk::PhysicalDevice >          physicalDevices;
	std::vector<Renderer*          >          renderers;
	std::vector<VmaAllocator       >          allocators;
	std::vector<GPUQueues          >          deviceQueues;
	std::vector<QueueFamilyIndices >          queueFamilyIndices;
	//

	size_t currentFrame = 0;

private:

	std::vector<const char*> validationLayers = {
		#if RDX_ENABLE_VK_VALIDATION_LAYERS
		"VK_LAYER_KHRONOS_validation"
		#endif
	};

	void startup();

	void createInstance();

	void createRenderer(int deviceIndex);
	int createDevice(int window);
	void createAllocator(int deviceIndex);

	void runLoop();
	bool shouldLoop();
	void runLoopIteration();



};

