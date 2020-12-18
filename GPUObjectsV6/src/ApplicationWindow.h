#pragma once


#include "ConfigSystem.h"
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

	//// application spacific - will move to other class after refactor - not sure about render pass manager yet


		// manager objects

	GraphicsPipeline* pipelineCreator;
	RenderPassManager* renderPassManager;


	vk::Instance instance = nullptr;



	vk::Device device = nullptr;
	vk::PhysicalDevice physicalDevice = nullptr;
	VmaAllocator allocator;


	GPUQueues deviceQueues;
	QueueFamilyIndices queueFamilyIndices;

	const int MAX_FRAMES_IN_FLIGHT = 3;


	//// window spacific - will keep here after refactor


	/// <summary>
	/// 
	/// </summary>
	/// <returns>weather to abort the frame</returns>
	bool getDrawable();
	void presentDrawable();

	Camera camera;

	GLFWwindow* window = nullptr;

	// Swap Chain

	vk::SwapchainKHR swapChain = nullptr;
	std::vector<VkImage> swapChainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<vk::ImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;


	VkFormat depthBufferFormat;

	VkFormat albedoFormat;
	VkFormat normalFormat;
	VkFormat aoFormat;
	

	/* Manualy created Frame Buffer Images */


	//GBuffer
	Image* gbuffer_albedo_metallic;
	Image* gbuffer_normal_roughness;
	Image* gbuffer_ao;
	Image* depthImage;

	//Deferred 
	//Image* deferred_colorAttachment; - right now befoe adding post processing passes the deferred writes directly to swap chain




	vk::SurfaceKHR surface;

	uint32_t currentSurfaceIndex;
	size_t currentFrame = 0;



	// Synchronization objects

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	bool framebufferResized = false;

	void recreateSwapchain();
private:

	//// application spacific - will move to other class after refactor

	void createInstance();
	void createDevice();


	std::vector<const char*> validationLayers = {
		#if RDX_ENABLE_VK_VALIDATION_LAYERS
		"VK_LAYER_KHRONOS_validation"
		#endif
	};

	//// window spacific - will keep here after refactor


	void cleanupSwapchain();

	void runWindowLoop();

	void destroyWindow();

	void createWindow();

	void makeWindwWithMode(ConfigSystem::Config::Window& winConfig, GLFWmonitor* monitor);


	void createSurface();


	void createSwapchain();

	void createSwapchainImageViews();
	void createFrameBufferImages();

	void createFramebuffers();

	void createSemaphores();


};

