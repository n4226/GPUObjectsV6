#pragma once

#include <vulkan/vulkan.hpp>
#include "GPUSelector.h"
#include "ApplicationWindow.h"

class Renderer
{
public:

	Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window);
	~Renderer();

	void renderFrame();

private:

	void createRenderResources();

	void createStaticRenderCommands();

	vk::Device& device;
	vk::PhysicalDevice& physicalDevice;
	WindowManager& window;

	// render resources

	VkCommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

};

