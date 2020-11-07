#pragma once


#include <vulkan/vulkan.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <optional>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}


};

struct GPUQueues {
	vk::Queue graphics;
	vk::Queue resourceTransfer;
	vk::Queue presentation;
};

class GPUSelector
{
public:
	static vk::PhysicalDevice primaryGPU(vk::Instance instance, vk::SurfaceKHR surface);

	static QueueFamilyIndices gpuQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	static SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

	static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

private: 
	static bool gpuSutable(const vk::PhysicalDevice device, vk::SurfaceKHR surface);

};




