#include "Renderer.h"

Renderer::Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window) : device(device), physicalDevice(physicalDevice), window(window)
{
	createRenderResources();
	createStaticRenderCommands();
}

Renderer::~Renderer()
{
	device.destroyCommandPool(commandPool);

	delete vertBuffer;
	delete indexBuffer;

	for (auto buffer : uniformBuffers)
	{
		delete buffer;
	}

	device.destroyDescriptorPool(descriptorPool);
}

void Renderer::createRenderResources()
{
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

	commandBuffers.resize(window.swapChainFramebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.commandPool = commandPool;
	// MARK: becuase of the constant errors i'm adding this
	

	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
	
	commandBuffers = device.allocateCommandBuffers(allocInfo);
	/*VkCommandBufferAllocateInfo newAllocInfo = allocInfo;

	std::vector<VkCommandBuffer> tempBuffers;
	tempBuffers.resize(commandBuffers.size());

	vkAllocateCommandBuffers(device, &newAllocInfo, tempBuffers.data());

	for (size_t i = 0; i < tempBuffers.size(); i++)
	{
		commandBuffers[i] = tempBuffers[i];
	}*/

	// create descriptor pool

	createDescriptorPoolAndSets();



}

void Renderer::createDescriptorPoolAndSets()
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(window.swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	poolInfo.maxSets = static_cast<uint32_t>(window.swapChainImages.size());

	descriptorPool = device.createDescriptorPool({ poolInfo });


	std::vector<vk::DescriptorSetLayout> layouts(window.swapChainImages.size(), window.pipelineCreator->terrianDescriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{};
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(window.swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	VkDescriptorSetAllocateInfo c_allocInfo = allocInfo;

	descriptorSets.resize(window.swapChainImages.size());

	vkAllocateDescriptorSets(device, &c_allocInfo, descriptorSets.data());


}

void Renderer::createStaticRenderCommands()
{

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



	for (size_t i = 0; i < commandBuffers.size(); i++) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		commandBuffers[i].begin(beginInfo);

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
		commandBuffers[i].beginRenderPass(&renderPassInfo,vk::SubpassContents::eInline);

		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, window.pipelineCreator->terrianPipelineLayout, 0, { descriptorSets[i] }, {});

		// encode commands 

		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, window.pipelineCreator->trianglePipeline);

		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, window.pipelineCreator->graphicsPipeline);

		commandBuffers[i].bindVertexBuffers(0, { vertBuffer->vkItem }, { 0 });
		commandBuffers[i].bindIndexBuffer({ indexBuffer->vkItem }, 0, vk::IndexType::eUint32);

		//commandBuffers[i].draw(vertices.size(), 1, 0, 0);
		commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		commandBuffers[i].endRenderPass();

		// end encoding

		commandBuffers[i].end();


	}
}


void Renderer::renderFrame()
{

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

	// submit frame

	vk::SubmitInfo submitInfo{};

	std::vector<vk::Semaphore> waitSemaphores = { window.imageAvailableSemaphore };
	std::vector<vk::PipelineStageFlags> waitStages = { vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput) };
	submitInfo.waitSemaphoreCount = waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.setWaitDstStageMask(waitStages);

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[window.currentSurfaceIndex];

	std::vector<vk::Semaphore> signalSemaphores = { window.renderFinishedSemaphore };
	submitInfo.setSignalSemaphores(signalSemaphores);


	// submit queue

	window.deviceQueues.graphics.submit({ submitInfo }, nullptr);
}
