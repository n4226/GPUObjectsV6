#pragma once

#include <vulkan/vulkan.hpp>
#include "GPUSelector.h"
#include "ApplicationWindow.h"
#include "Buffer.h"
#include "Mesh.h"
//#include "vk_mem_alloc.h"

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

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	VmaAllocator allocator;

	// temp vars

	Buffer* vertBuffer;
	Buffer* indexBuffer;
};

