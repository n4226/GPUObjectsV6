#pragma once

#include <vulkan/vulkan.hpp>

class RenderPassManager
{
public:

	RenderPassManager(vk::Device device, VkFormat swapChainImageFormat, VkFormat depthBufferFormat);
	~RenderPassManager();

	VkRenderPass renderPass;
private:

	void createMainRenderPass();

	VkFormat swapChainImageFormat;
	VkFormat depthBufferFormat;
	vk::Device device;
};

