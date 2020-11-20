#pragma once


#include "pch.h"
#include <GLFW/glfw3.h>
#include "GPUSelector.h"
#include "PipelineCreator.h"
#include "Camera.h"
#include "TrianglePipeline.h"
#include "TerrainPipeline.h"
#include "environment.h"

class WorldScene;

class WindowManager
{
public:
	WindowManager();
	~WindowManager();


	void getDrawable();
	void presentDrawable();


	Camera camera;

	// manager objects

	PipelineCreator* pipelineCreator;
	RenderPassManager* renderPassManager;


	GLFWwindow* window = nullptr;

	vk::Instance instance = nullptr;

	vk::Device device = nullptr;
	vk::PhysicalDevice physicalDevice = nullptr;

	// Swap Chain

	vk::SwapchainKHR swapChain = nullptr;
	std::vector<VkImage> swapChainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<vk::ImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;


	GPUQueues deviceQueues;
	QueueFamilyIndices queueFamilyIndices;

	vk::SurfaceKHR surface;

	uint32_t currentSurfaceIndex;



	// Synchronization objects

	vk::Semaphore imageAvailableSemaphore;
	vk::Semaphore renderFinishedSemaphore;

private:

	void runWindowLoop();

	void destroyWindow();

	void createWindow();

	void createInstance();

	void createSurface();

	void createDevice();

	void createSwapchain();

	void createSwapchainImageViews();

	void createFramebuffers();

	void createSemaphores();


	std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

};

