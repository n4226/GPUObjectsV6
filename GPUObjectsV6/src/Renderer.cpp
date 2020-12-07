#include "pch.h"
#include "Renderer.h"

Renderer::Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window) : device(device), physicalDevice(physicalDevice), window(window)
{
	PROFILE_FUNCTION
	createRenderResources();
	createUniformsAndDescriptors();

	createDynamicRenderCommands(device, window);
}

Renderer::~Renderer()
{
	PROFILE_FUNCTION

	for (auto pool : dynamicCommandPools)
	{
		device.destroyCommandPool(pool);
	}

	for (auto buffer : uniformBuffers)
	{
		delete buffer;
	}

	delete globalMeshStagingBuffer;
	
	for (auto buff : threadLocalGlobalMeshStagingBuffers) {
		buff->vertBuffer->unmapMemory();
		buff->indexBuffer->unmapMemory();
		delete buff;
	}

	delete globalMeshBuffer;
	for (auto buffer : globalModelBuffers)
	{
		delete buffer;
	}
	
	for (auto buff : threadLocalGlobalModelStagingBuffers) {
		buff->unmapMemory();
		delete buff;
	}

	delete globalModelBufferStaging;

	delete gloablIndAllocator;
	delete gloablVertAllocator;
	delete globalModelBufferAllocator;

	device.destroyDescriptorPool(descriptorPool);
}

void Renderer::createRenderResources()
{
	PROFILE_FUNCTION

	allocator = window.allocator;

	
#pragma region Create Global vert and in

	VkDeviceSize vCount =      70'000'000;
	VkDeviceSize indexCount =  220'000'000;

	makeGlobalMeshBuffers(vCount, indexCount);
	 
#pragma endregion

	createDescriptorPoolAndSets();

	// create depth attatchment(s)
	createDepthAttatchments();

}

void Renderer::makeGlobalMeshBuffers(const VkDeviceSize& vCount, const VkDeviceSize& indexCount)
{
	BufferCreationOptions options =
	{ ResourceStorageType::cpu,{ vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc }, vk::SharingMode::eConcurrent,
		{ window.queueFamilyIndices.graphicsFamily.value(), window.queueFamilyIndices.resourceTransferFamily.value() } };

	globalMeshStagingBuffer = new BindlessMeshBuffer(device, allocator, options, vCount, indexCount);


	//TODO Extract this comman code somewhere

	auto workerThreads = std::thread::hardware_concurrency() * 0;

	threadLocalGlobalMeshStagingBuffers.resize(workerThreads);

	auto threadMeshModelInd = freeThreadLocalGlobalMeshandModelStagingBufferIndicies.lock();

	for (size_t thread = 0; thread < workerThreads; thread++)
	{
		threadMeshModelInd->push_back(thread);
		threadLocalGlobalMeshStagingBuffers[thread] = new BindlessMeshBuffer(device, allocator, options, vCount, indexCount);
		threadLocalGlobalMeshStagingBuffers[thread]->indexBuffer->mapMemory();
		threadLocalGlobalMeshStagingBuffers[thread]->vertBuffer->mapMemory();
	}


	options.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc;


	globalModelBufferStaging = new Buffer(device, allocator, sizeof(ModelUniforms) * maxModelUniformDescriptorArrayCount, options);

	threadLocalGlobalModelStagingBuffers.resize(workerThreads);
	for (size_t thread = 0; thread < workerThreads; thread++)
	{
		threadLocalGlobalModelStagingBuffers[thread] = new Buffer(device, allocator, sizeof(ModelUniforms) * maxModelUniformDescriptorArrayCount, options);
		threadLocalGlobalModelStagingBuffers[thread]->mapMemory();
	}

	options.storage = ResourceStorageType::gpu;

	options.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst;
	globalModelBuffers = {
		new Buffer(device, allocator, sizeof(ModelUniforms) * maxModelUniformDescriptorArrayCount, options),
		new Buffer(device, allocator, sizeof(ModelUniforms) * maxModelUniformDescriptorArrayCount, options),
	};

	options.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
	globalMeshBuffer = new BindlessMeshBuffer(device, allocator, options, vCount, indexCount);

	gloablVertAllocator = new VaribleIndexAllocator(globalMeshBuffer->vCount);
	gloablIndAllocator = new VaribleIndexAllocator(globalMeshBuffer->indexCount);

	globalModelBufferAllocator = new IndexAllocator(maxModelUniformDescriptorArrayCount, sizeof(ModelUniforms));
}

void Renderer::createDepthAttatchments()
{

}

void Renderer::createDescriptorPoolAndSets()
{
	PROFILE_FUNCTION

	VkDescriptorPoolSize globalUniformPoolSize{};
	globalUniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalUniformPoolSize.descriptorCount = static_cast<uint32_t>(window.swapChainImages.size());

	// the total max number of this descriptor allocated - if 2 sets and each one has 2 of this descriptor than thes would have to be 4 in order to allocate both sets
	VkDescriptorPoolSize modelUniformPoolSize{};
	modelUniformPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	modelUniformPoolSize.descriptorCount = static_cast<uint32_t>(window.swapChainImages.size());

	std::array<VkDescriptorPoolSize, 2> poolSizes = { globalUniformPoolSize, modelUniformPoolSize };

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	poolInfo.maxSets = static_cast<uint32_t>(window.swapChainImages.size());

	descriptorPool = device.createDescriptorPool({ poolInfo });


	std::vector<vk::DescriptorSetLayout> layouts(window.swapChainImages.size(), window.pipelineCreator->descriptorSetLayouts[0]);
	vk::DescriptorSetAllocateInfo allocInfo{};
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(window.swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	VkDescriptorSetAllocateInfo c_allocInfo = allocInfo;

	descriptorSets.resize(window.swapChainImages.size());

	vkAllocateDescriptorSets(device, &c_allocInfo, descriptorSets.data());


}

void Renderer::createDynamicRenderCommands(vk::Device& device, WindowManager& window)
{
	VkHelpers::createPoolsAndCommandBufffers
	(device, dynamicCommandPools, dynamicCommandBuffers, window.swapChainImageViews.size(), window.queueFamilyIndices.graphicsFamily.value(), vk::CommandBufferLevel::ePrimary);
}

void Renderer::createUniformsAndDescriptors()
{
	PROFILE_FUNCTION
	

	// make uniforms

	VkDeviceSize uniformBufferSize = sizeof(SceneUniforms); //sizeof(TriangleUniformBufferObject);

	uniformBuffers.resize(window.swapChainImages.size());

	BufferCreationOptions uniformOptions = { ResourceStorageType::cpuToGpu,{vk::BufferUsageFlagBits::eUniformBuffer}, vk::SharingMode::eExclusive };

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i] = new Buffer(device, allocator, uniformBufferSize, uniformOptions);
	}
	
	// set up descriptors 

	for (size_t i = 0; i < window.swapChainImages.size(); i++) {

		VkDescriptorBufferInfo globalUniformBufferInfo{};
		globalUniformBufferInfo.buffer = uniformBuffers[i]->vkItem;
		globalUniformBufferInfo.offset = 0;
		globalUniformBufferInfo.range = sizeof(SceneUniforms);

		VkDescriptorBufferInfo modelUniformBufferInfo{};
		//TODO fix this to actuall non staging buffer
		modelUniformBufferInfo.buffer = globalModelBufferStaging->vkItem;
		modelUniformBufferInfo.offset = 0;
		modelUniformBufferInfo.range = sizeof(ModelUniforms) * maxModelUniformDescriptorArrayCount;

		VkWriteDescriptorSet globalUniformDescriptorWrite{};
		globalUniformDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		globalUniformDescriptorWrite.dstSet = descriptorSets[i];
		globalUniformDescriptorWrite.dstBinding = 0;
		globalUniformDescriptorWrite.dstArrayElement = 0;

		globalUniformDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		globalUniformDescriptorWrite.descriptorCount = 1;

		globalUniformDescriptorWrite.pBufferInfo = &globalUniformBufferInfo;
		globalUniformDescriptorWrite.pImageInfo = nullptr; // Optional
		globalUniformDescriptorWrite.pTexelBufferView = nullptr; // Optional

		VkWriteDescriptorSet modelUniformsDescriptorWrite{};
		modelUniformsDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		modelUniformsDescriptorWrite.dstSet = descriptorSets[i];
		modelUniformsDescriptorWrite.dstBinding = 1;
		modelUniformsDescriptorWrite.dstArrayElement = 0;

		modelUniformsDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		modelUniformsDescriptorWrite.descriptorCount = 1;

		modelUniformsDescriptorWrite.pBufferInfo = &modelUniformBufferInfo;
		modelUniformsDescriptorWrite.pImageInfo = nullptr; // Optional
		modelUniformsDescriptorWrite.pTexelBufferView = nullptr; // Optional


		device.updateDescriptorSets({ globalUniformDescriptorWrite, modelUniformsDescriptorWrite }, {});
	}

}


void Renderer::renderFrame()
{
	PROFILE_FUNCTION



	// update uniform buffer

	SceneUniforms uniforms;

	uniforms.viewProjection = window.camera.viewProjection(window.swapchainExtent.width, window.swapchainExtent.height);
	uniformBuffers[window.currentSurfaceIndex]->tempMapAndWrite(&uniforms, 0, sizeof(uniforms));

	camFrustrom = new Frustum(uniforms.viewProjection);

#pragma region CreateRootCMDBuffer

	// create root cmd buffer

	device.resetCommandPool(dynamicCommandPools[window.currentSurfaceIndex], {});

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; //VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	dynamicCommandBuffers[window.currentSurfaceIndex].begin(beginInfo);


	// begin a render pass

	vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo.renderPass = window.renderPassManager->renderPass;
	renderPassInfo.framebuffer = window.swapChainFramebuffers[window.currentSurfaceIndex];

	renderPassInfo.renderArea = vk::Rect2D({ 0, 0 }, window.swapchainExtent);


		//VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		const std::array<float,4> clearComponents = { 0.0f, 0.0f, 0.2f, 1.0f };

		std::array<vk::ClearValue, 2> clearColors = {
			vk::ClearValue(vk::ClearColorValue(clearComponents)),
			vk::ClearValue(vk::ClearDepthStencilValue({1.f,0}))
		};

	renderPassInfo.setClearValues(clearColors);

	VkRenderPassBeginInfo info = renderPassInfo;

	//vkCmdBeginRenderPass(commandBuffers[i], &info, VK_SUBPASS_CONTENTS_INLINE);
	dynamicCommandBuffers[window.currentSurfaceIndex].beginRenderPass(&renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);


#pragma endregion

	// run terrain system draw

	auto generatedTerrainCmds = terrainSystem->renderSystem(0);


	// exicute indirect commands

	dynamicCommandBuffers[window.currentSurfaceIndex].executeCommands({ 1, generatedTerrainCmds });

	// end encoding 

	dynamicCommandBuffers[window.currentSurfaceIndex].endRenderPass();
	dynamicCommandBuffers[window.currentSurfaceIndex].end();

	// submit frame

	submitFrameQueue(&dynamicCommandBuffers[window.currentSurfaceIndex],1);
}

void Renderer::submitFrameQueue(vk::CommandBuffer* buffers,uint32_t bufferCount)
{
	vk::SubmitInfo submitInfo{};

	std::vector<vk::Semaphore> waitSemaphores = { window.imageAvailableSemaphores[window.currentFrame] };
	std::vector<vk::PipelineStageFlags> waitStages = { vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput) };
	submitInfo.waitSemaphoreCount = waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.setWaitDstStageMask(waitStages);

	submitInfo.commandBufferCount = bufferCount;
	submitInfo.pCommandBuffers = buffers;

	std::vector<vk::Semaphore> signalSemaphores = { window.renderFinishedSemaphores[window.currentFrame] };
	submitInfo.setSignalSemaphores(signalSemaphores);


	vkResetFences(device, 1, &window.inFlightFences[window.currentFrame]);

	// submit queue

	window.deviceQueues.graphics.submit({ submitInfo }, window.inFlightFences[window.currentFrame]);
}
