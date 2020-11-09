#include "Renderer.h"

Renderer::Renderer(vk::Device& device, vk::PhysicalDevice& physicalDevice, WindowManager& window) : device(device), physicalDevice(physicalDevice), window(window)
{
	createRenderResources();
	createStaticRenderCommands();
}

Renderer::~Renderer()
{
	device.destroyCommandPool(commandPool);
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
}

void Renderer::createStaticRenderCommands()
{

	const std::vector<TriangleVert> vertices = {
		{{0.0f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}}
	};
	VkDeviceSize size = sizeof(TriangleVert) * vertices.size();

	BufferCreationOptions options = { BufferCreationOptions::cpuToGpu,{vk::BufferUsageFlagBits::eVertexBuffer}, vk::SharingMode::eExclusive };

	Buffer stageVertBuffer = 
		Buffer(device,allocator, size, options);

	stageVertBuffer.mapMemory();

	memcpy(stageVertBuffer.mappedData, vertices.data(),(size_t)stageVertBuffer.size);

	// not necicary on windows with big 3 drivers see vma docs on cash flushing for info
	//vmaFlushAllocation(allocator,stageVertBuffer.allocation,);

	stageVertBuffer.unmapMemory();


	for (size_t i = 0; i < commandBuffers.size(); i++) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
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

		// encode commands 

		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, window.pipelineCreator->trianglePipeline);

		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, window.pipelineCreator->graphicsPipeline);

		commandBuffers[i].bindVertexBuffers(0, { stageVertBuffer.vkItem }, { 0 });

		commandBuffers[i].draw(vertices.size(), 1, 0, 0);

		commandBuffers[i].endRenderPass();

		// end encoding

		commandBuffers[i].end();


	}
}


void Renderer::renderFrame()
{
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
