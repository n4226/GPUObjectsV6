#pragma once

#include "../../../pch.h"
#include "../../vulkanAbstractions/Buffer.h"
#include "../../dataObjects/Mesh.h"
#include "../../dataObjects/uniforms.h"
#include "../../dataObjects/Transform.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "../../dataObjects/Camera.h"
#include "../../spacificVulkanImplementations/renderPipelines/PipelineCreator.h"
#include "../terrainSystem/TerrainSystem.h"
#include "memory/VaribleIndexAllocator.h"
#include "memory/IndexAllocator.h"
#include "../../spacificVulkanImplementations/resources/ResourceTransferTask.h"
#include "../../spacificVulkanImplementations/renderPipelines/GBufferComputePipeline.h"

class Renderer
{
public:

	Renderer(Application& app, vk::Device device, vk::PhysicalDevice physicalDevice, VmaAllocator allocator, std::vector<WindowManager*>& windows, GPUQueues& deviceQueues, QueueFamilyIndices& queueFamilyIndices);
	void createAllResources();
	~Renderer();

	void beforeRenderScene();
	void renderFrame(WindowManager& window);
	
	// systems
	TerrainSystem* terrainSystem;
	WorldScene* world;

	// handles
	vk::Device device;
	vk::PhysicalDevice physicalDevice;
	VmaAllocator allocator;
	GPUQueues& deviceQueues;
	QueueFamilyIndices& queueFamilyIndices;

	std::vector<WindowManager*> windows;

	std::vector<Buffer*> uniformBuffers;

	// dindless vars

	VaribleIndexAllocator* gloablVertAllocator;
	VaribleIndexAllocator* gloablIndAllocator;

	BindlessMeshBuffer* globalMeshBuffer;

	// thye main thread staging buffer
	BindlessMeshBuffer* globalMeshStagingBuffer;

	libguarded::shared_guarded < std::vector<size_t>> freeThreadLocalGlobalMeshandModelStagingBufferIndicies;
	libguarded::shared_guarded < std::unordered_map<std::thread::id,size_t>> ThreadLocalGlobalMeshandModelStagingBufferThreadIndicies;

	std::vector<BindlessMeshBuffer*> threadLocalGlobalMeshStagingBuffers;


	IndexAllocator* globalModelBufferAllocator;
	std::array<Buffer*,2> globalModelBuffers;

	size_t gpuUnactiveGlobalModelBuffer = 1;
	size_t gpuActiveGlobalModelBuffer = 0;

	Buffer* globalModelBufferStaging;

	std::vector<Buffer*> threadLocalGlobalModelStagingBuffers;
private:

	void createRenderResources();

	void makeGlobalMeshBuffers(const VkDeviceSize& vCount, const VkDeviceSize& indexCount);

	void createDescriptorPoolAndSets();

	void createUniformsAndDescriptors();

	void updateLoadTimeDescriptors(WindowManager& window);
	void updateRunTimeDescriptors(WindowManager& window);

	void createDynamicRenderCommands();

	void submitFrameQueue(WindowManager& window, vk::CommandBuffer* buffers, uint32_t bufferCount);



	void encodeDeferredPass(WindowManager& window);

	void encodeGBufferPass(WindowManager& window);

	void updateCameraUniformBuffer();

	// render resources


	std::vector<std::vector<vk::CommandPool  >> dynamicCommandPools;
	std::vector<std::vector<vk::CommandBuffer>> dynamicCommandBuffers;



	vk::DescriptorPool descriptorPool;
	// first dimension is windows second is surface index
	std::vector<std::vector<VkDescriptorSet>> descriptorSets;


	// deferred pass

	Buffer* deferredPassVertBuff;
	size_t deferredPassBuffIndexOffset;


	vk::DescriptorPool deferredDescriptorPool;
	std::vector<std::vector<VkDescriptorSet>> deferredDescriptorSets;

	friend TerrainSystem;
	friend Application;


	std::vector<Frustum> camFrustroms;

	// handles

	Application& app;

};

