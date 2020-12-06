#pragma once

#include "pch.h"
#include "Buffer.h"
#include "Mesh.h"
#include "uniforms.h"
#include "Transform.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Camera.h"
#include "PipelineCreator.h"
#include "TerrainSystem.h"
#include "VaribleIndexAllocator.h"
#include "IndexAllocator.h"
#include "ResourceTransferTask.h"


class Renderer
{
public:

	Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window);
	~Renderer();

	void renderFrame();


	
	// systems
	TerrainSystem* terrainSystem;


	vk::Device& device;
	vk::PhysicalDevice& physicalDevice;
	WindowManager& window;

	VmaAllocator allocator;

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
	size_t gpuActiveGlobalModelBuffer = 0;

	Buffer* globalModelBufferStaging;

	std::vector<Buffer*> threadLocalGlobalModelStagingBuffers;
private:

	void createRenderResources();

	void makeGlobalMeshBuffers(const VkDeviceSize& vCount, const VkDeviceSize& indexCount);

	void createDepthAttatchments();

	void createDescriptorPoolAndSets();

	void createUniformsAndDescriptors();

	void createDynamicRenderCommands(vk::Device& device, WindowManager& window);

	void submitFrameQueue(vk::CommandBuffer* buffers, uint32_t bufferCount);


	// render resources


	std::vector<vk::CommandPool> dynamicCommandPools;
	std::vector<vk::CommandBuffer> dynamicCommandBuffers;



	vk::DescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;


	friend TerrainSystem;

	Frustum* camFrustrom;

};

