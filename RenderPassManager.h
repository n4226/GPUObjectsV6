#pragma once

#include <vulkan/vulkan.hpp>

class RenderPassManager
{
public:

	RenderPassManager();
	RenderPassManager(vk::Device device, VkFormat swapChainImageFormat);
	~RenderPassManager();

	VkRenderPass renderPass;
private:

	void createMainRenderPass();

	VkFormat swapChainImageFormat;

	vk::Device device;
};

