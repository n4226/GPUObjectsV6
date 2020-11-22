#include "pch.h"
#include "Renderer.h"

Renderer::Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window) : device(device), physicalDevice(physicalDevice), window(window)
{
	PROFILE_FUNCTION
	createRenderResources();
	createStaticRenderCommands();

	createDynamicRenderCommands(device, window);
}

Renderer::~Renderer()
{
	PROFILE_FUNCTION
	device.destroyCommandPool(commandPool);

	delete vertBuffer;
	delete indexBuffer;

	for (auto buffer : uniformBuffers)
	{
		delete buffer;
	}

	delete globalVerticiesStaging;
	delete globalVerticies;
	delete globalIndiciesStaging;
	delete globalIndicies;

	device.destroyDescriptorPool(descriptorPool);
}

void Renderer::createRenderResources()
{
	PROFILE_FUNCTION
	// allocator

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	allocatorInfo.physicalDevice = physicalDevice;
	allocatorInfo.device = device;
	allocatorInfo.instance = window.instance;

	vmaCreateAllocator(&allocatorInfo, &allocator);

	// command pool

	vk::CommandPoolCreateInfo poolInfo{};

	poolInfo.queueFamilyIndex = window.queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = vk::CommandPoolCreateFlags(); // Optional

	commandPool = device.createCommandPool(poolInfo);

	// command buffers

	// MARK: becuase of the constant errors i'm adding this

	staticCommandBuffers.resize(window.swapChainFramebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.commandPool = commandPool;
	

	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = (uint32_t)staticCommandBuffers.size();
	
	staticCommandBuffers = device.allocateCommandBuffers(allocInfo);
	
#pragma region Create Global vert and in

	BufferCreationOptions options = { BufferCreationOptions::cpu,{vk::BufferUsageFlagBits::eVertexBuffer}, vk::SharingMode::eExclusive };

	globalVerticiesStaging = new Buffer(device,allocator,6000000,options);

	options.storage = BufferCreationOptions::gpu;

	globalVerticies = new Buffer(device,allocator,6000000,options);

	options.usage = vk::BufferUsageFlagBits::eIndexBuffer;
	options.storage = BufferCreationOptions::cpu;

	globalIndiciesStaging = new Buffer(device,allocator,6000000,options);

	options.storage = BufferCreationOptions::gpu;

	globalIndicies = new Buffer(device,allocator,6000000,options);

	gloablVertAllocator = new VaribleIndexAllocator(6000000);
	gloablIndAllocator =  new VaribleIndexAllocator(6000000);
	 
#pragma endregion

	createDescriptorPoolAndSets();

}

void Renderer::createDescriptorPoolAndSets()
{
	PROFILE_FUNCTION

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(window.swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	poolInfo.maxSets = static_cast<uint32_t>(window.swapChainImages.size());

	descriptorPool = device.createDescriptorPool({ poolInfo });


	std::vector<vk::DescriptorSetLayout> layouts(window.swapChainImages.size(), window.pipelineCreator->descriptorSetLayout);
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

void Renderer::createStaticRenderCommands()
{
	PROFILE_FUNCTION
	const std::vector<TriangleVert> vertices = {
		{{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.f}, {.5f, .5f, 0.0f}}
	};

	const std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	VkDeviceSize vertBuffSize = sizeof(TriangleVert) * vertices.size();
	VkDeviceSize indiciesBuffSize = sizeof(uint32_t) * indices.size();

	BufferCreationOptions options = { BufferCreationOptions::cpu,{vk::BufferUsageFlagBits::eVertexBuffer}, vk::SharingMode::eExclusive };

	vertBuffer =
		Buffer::StageAndCreatePrivate(device,window.deviceQueues.graphics, commandPool, allocator, vertBuffSize, vertices.data(), options);

	options.usage = { vk::BufferUsageFlagBits::eIndexBuffer };
	
	indexBuffer =
		Buffer::StageAndCreatePrivate(device, window.deviceQueues.graphics, commandPool, allocator, indiciesBuffSize, indices.data(), options);


	BufferCreationOptions options2 = { BufferCreationOptions::cpuToGpu,{vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer}, vk::SharingMode::eExclusive };

	mesh = Mesh::quad();
	meshBuffer = new MeshBuffer(device, allocator, options2,mesh);
	meshBuffer->writeMeshToBuffer(true);
	// make uniforms

	VkDeviceSize uniformBufferSize = sizeof(TriangleUniformBufferObject);

	uniformBuffers.resize(window.swapChainImages.size());

	BufferCreationOptions uniformOptions = { BufferCreationOptions::cpuToGpu,{vk::BufferUsageFlagBits::eUniformBuffer}, vk::SharingMode::eExclusive };

	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		uniformBuffers[i] = new Buffer(device, allocator, uniformBufferSize, uniformOptions);
	}
	
	// set up descriptors 

	for (size_t i = 0; i < window.swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i]->vkItem;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(TriangleUniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;

		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		device.updateDescriptorSets({ descriptorWrite }, {});
	}



	for (size_t i = 0; i < staticCommandBuffers.size(); i++) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		staticCommandBuffers[i].begin(beginInfo);

		// begin a render pass

		vk::RenderPassBeginInfo renderPassInfo{};
		renderPassInfo.renderPass = window.renderPassManager->renderPass;
		renderPassInfo.framebuffer = window.swapChainFramebuffers[i];

		renderPassInfo.renderArea = vk::Rect2D( { 0, 0 }, window.swapchainExtent);
		
		//VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		const std::array<float,4> clearComponents = { 0.0f, 0.0f, 0.2f, 1.0f };
		vk::ClearValue clearColor = vk::ClearValue(vk::ClearColorValue(clearComponents));
		renderPassInfo.setClearValues(clearColor);

		VkRenderPassBeginInfo info = renderPassInfo;

		//vkCmdBeginRenderPass(commandBuffers[i], &info, VK_SUBPASS_CONTENTS_INLINE);
		staticCommandBuffers[i].beginRenderPass(&renderPassInfo,vk::SubpassContents::eInline);

		staticCommandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, window.pipelineCreator->pipelineLayout, 0, { descriptorSets[i] }, {});

		// encode commands 

		staticCommandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, window.pipelineCreator->vkItem);

		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, window.pipelineCreator->graphicsPipeline);

		//commandBuffers[i].bindVertexBuffers(0, { vertBuffer->vkItem }, { 0 });
		//commandBuffers[i].bindIndexBuffer({ indexBuffer->vkItem }, 0, vk::IndexType::eUint32);
		meshBuffer->bindVerticiesIntoCommandBuffer(staticCommandBuffers[i], 0);
		meshBuffer->bindIndiciesIntoCommandBuffer(staticCommandBuffers[i]);

		//commandBuffers[i].draw(vertices.size(), 1, 0, 0);
		//commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		staticCommandBuffers[i].drawIndexed(static_cast<uint32_t>(meshBuffer->baseMesh->indicies.size()), 1, 0, 0, 0);

		staticCommandBuffers[i].endRenderPass();

		// end encoding

		staticCommandBuffers[i].end();


	}
}


void Renderer::renderFrame()
{
	PROFILE_FUNCTION
	// update frame buffer

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	static auto quadtransform = Transform();
	quadtransform.position.z = 1;
	//window.camera.transform.position.y = 10;


	glm::vec3 axis = { 0,1,0 };
	quadtransform.rotation = glm::angleAxis(glm::radians(60 * sin(time)), axis);

	TriangleUniformBufferObject ubo;

	ubo.model = quadtransform.matrix();
	ubo.viewProjection = window.camera.viewProjection(window.swapchainExtent.width, window.swapchainExtent.height);

	//Using a UBO this way is not the most efficient way to pass frequently changing values to the shader. A more efficient way to pass a small buffer of data to shaders are push constants. We may look at these in a future chapter.
	uniformBuffers[window.currentSurfaceIndex]->tempMapAndWrite(&ubo, sizeof(ubo));


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
	const std::array<float, 4> clearComponents = { 0.0f, 0.0f, 0.2f, 1.0f };
	vk::ClearValue clearColor = vk::ClearValue(vk::ClearColorValue(clearComponents));
	renderPassInfo.setClearValues(clearColor);

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
