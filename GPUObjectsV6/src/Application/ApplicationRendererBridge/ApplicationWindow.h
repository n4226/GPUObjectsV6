#pragma once


#include "../ConfigSystem.h"
#include "../../pch.h"
#include <GLFW/glfw3.h>
#include "../../RenderEngine/vulkanAbstractions/GPUSelector.h"
#include "../../RenderEngine/spacificVulkanImplementations/renderPipelines/PipelineCreator.h"
#include "../../RenderEngine/dataObjects/Camera.h"
#include "../../RenderEngine/spacificVulkanImplementations/renderPipelines/concrete/TrianglePipeline.h"
#include "../../RenderEngine/spacificVulkanImplementations/renderPipelines/concrete/TerrainPipeline.h"
#include "../../RenderEngine/spacificVulkanImplementations/renderPipelines/concrete/gpuDriven/GPUGenCommandsPipeline.h"
#include "../../RenderEngine/spacificVulkanImplementations/renderPipelines/GBufferComputePipeline.h"
#include "../../environment.h"

class WorldScene;
class Renderer;
class Application;

class WindowManager
{
public:
	WindowManager(Application* app, size_t globalIndex);
	~WindowManager();

	void finishInit();

	size_t globalIndex;

	//// application spacific - will move to other class after refactor - not sure about render pass manager yet


		// manager objects

	RenderPassManager* renderPassManager;
	GraphicsPipeline* pipelineCreator;
	GPUGenCommandsPipeline* gpuGenPipe;

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

	DeferredPass* deferredPass;



	vk::SurfaceKHR surface;

	uint32_t currentSurfaceIndex;



	// Synchronization objects

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	bool framebufferResized = false;

	void recreateSwapchain();

	// refrences =

	vk::Device device;
	Renderer* renderer;
	size_t indexInRenderer;

	Application& app;


private:

	//// application spacific - will move to other class after refactor


	

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

