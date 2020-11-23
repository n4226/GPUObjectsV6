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
	void createAllocator();
	~WindowManager();

	/// <summary>
	/// 
	/// </summary>
	/// <returns>weather to abort the frame</returns>
	bool getDrawable();
	void presentDrawable();


	Camera camera;

	// manager objects

	PipelineCreator* pipelineCreator;
	RenderPassManager* renderPassManager;


	GLFWwindow* window = nullptr;

	vk::Instance instance = nullptr;

	vk::Device device = nullptr;
	vk::PhysicalDevice physicalDevice = nullptr;
	VmaAllocator allocator;

	// Swap Chain

	vk::SwapchainKHR swapChain = nullptr;
	std::vector<VkImage> swapChainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<vk::ImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkFormat depthBufferFormat;
	Image* depthImage;

	GPUQueues deviceQueues;
	QueueFamilyIndices queueFamilyIndices;

	vk::SurfaceKHR surface;

	uint32_t currentSurfaceIndex;
	size_t currentFrame = 0;


	const int MAX_FRAMES_IN_FLIGHT = 3;

	// Synchronization objects

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	bool framebufferResized = false;

	void recreateSwapchain();
private:

	void cleanupSwapchain();

	void runWindowLoop();

	void destroyWindow();

	void createWindow();

	void createInstance();

	void createSurface();

	void createDevice();

	void createSwapchain();

	void createSwapchainImageViews();
	void createDepthImage();

	void createFramebuffers();

	void createSemaphores();


	std::vector<const char*> validationLayers = {
		#if RDX_ENABLE_VK_VALIDATION_LAYERS
		"VK_LAYER_KHRONOS_validation"
		#endif
	};


};

