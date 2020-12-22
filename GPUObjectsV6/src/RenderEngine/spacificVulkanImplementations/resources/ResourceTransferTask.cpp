#include "pch.h"
#include "ResourceTransferTask.h"
#include "RenderEngine/systems/renderSystems/Renderer.h"




MarlSafeTicketLock::MarlSafeTicketLock(marl::Ticket ticket)
	: ticket(ticket)
{
	ticket.wait();
}

MarlSafeTicketLock::~MarlSafeTicketLock()
{
	ticket.done();
}



ResourceTransferer::ResourceTransferer(vk::Device device, Renderer& renderer)
	: device(device),  renderer(renderer), ticketQueue()
{

	vk::CommandPoolCreateInfo poolInfo{};

	// TODO: this is temporary switch queue back to correct one
	poolInfo.queueFamilyIndex = renderer.queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = vk::CommandPoolCreateFlags(); // Optional

	copyPool = device.createCommandPool(poolInfo);

	poolInfo.queueFamilyIndex = renderer.queueFamilyIndices.graphicsFamily.value();

	VkHelpers::allocateCommandBuffers(device, copyPool, &cmdBuffer, 1);
}

ResourceTransferer::~ResourceTransferer()
{
	ticketQueue.take().wait();
	device.destroyCommandPool(copyPool);
}



void ResourceTransferer::newTask(std::vector<Task>& tasks, std::function<void()> completionHandler,bool synchronus)
{
	PROFILE_FUNCTION

	//marl::schedule([]() {printf("transfer job"); });
	auto ticket = ticketQueue.take();
	
	if (synchronus)
		performTask(tasks, ticket, completionHandler);
	else {
		marl::schedule([tasks, ticket, completionHandler, this]() { performTask(tasks, ticket, completionHandler); });

	}
}


void ResourceTransferer::performTask(std::vector<Task> tasks, marl::Ticket ticket, std::function<void()> completionHandler)
{
	PROFILE_FUNCTION
	{
		//// will lock the ticket until the lock goes out of scope
		//TODO: fix this
		//MarlSafeTicketLock lock(ticket);

		ticket.wait();

		PROFILE_SCOPE("performTask Marl blocking")


		device.resetCommandPool(copyPool, {});

		cmdBuffer.begin({ { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } });

		// perform tasks
		for (Task& task : tasks) {
			
			switch (task.type)
			{
			case TaskType::bufferTransfers:
				performBufferTransferTask(task.bufferTransferTask);
				break;

			case TaskType::imageLayoutTransition:
				performImageLayoutTransitionTask(task.imageLayoutTransitonTask);
				break;

			case TaskType::bufferToImageCopyWithTransition:
				performBufferToImageCopyWithTransitionTask(task.bufferToImageCopyWithTransitionTask);
				break;

			//Graphics Queue


			case TaskType::generateMipMaps:
				performGenerateMipMapsTask(task.generateMipMapsTask);
				break;
			default:
				break;
			}

		}

		cmdBuffer.end();


		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		renderer.deviceQueues.graphics.submit(submitInfo, nullptr);

		//TODO: find better way than blocking the resource transfer marl queue

		renderer.deviceQueues.graphics.waitIdle();

		ticket.done();
	}

	
	// after ticket unlcok so other threads can access the vk resources
	//marl::blocking_call(completionHandler);
	completionHandler();
}


void ResourceTransferer::performBufferTransferTask(ResourceTransferer::BufferTransferTask& t)
{
	cmdBuffer.copyBuffer(t.srcBuffer, t.dstBuffer, t.regions);
}

void ResourceTransferer::performBufferToImageCopyWithTransitionTask(ResourceTransferer::BufferToImageCopyWithTransitionTask& t)
{

	ImageLayoutTransitionTask layoutTask;
	layoutTask.image = t.image;
	layoutTask.imageAspectMask = t.imageAspectMask;
	layoutTask.oldLayout = t.oldLayout;
	layoutTask.newLayout = vk::ImageLayout::eTransferDstOptimal;

	performImageLayoutTransitionTask(layoutTask);

	vk::BufferImageCopy region;

	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = t.imageAspectMask;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset.x = 0;
	region.imageOffset.y = 0;
	region.imageOffset.z = 0;
	region.imageExtent = t.imageSize;



	cmdBuffer.copyBufferToImage(
		t.buffer,
		t.image,
		vk::ImageLayout::eTransferDstOptimal,
		{region}
	);


	if (layoutTask.newLayout != t.finalLayout) {

		layoutTask.oldLayout = layoutTask.newLayout;
		layoutTask.newLayout = t.finalLayout;

		performImageLayoutTransitionTask(layoutTask);

	}
}

void ResourceTransferer::performImageLayoutTransitionTask(ResourceTransferer::ImageLayoutTransitionTask& t)
{
	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout = t.oldLayout;
	barrier.newLayout = t.newLayout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = t.image;
	barrier.subresourceRange.aspectMask = t.imageAspectMask;
	barrier.subresourceRange.baseMipLevel = t.baseMipLevel;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (t.oldLayout == vk::ImageLayout::eUndefined && t.newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits{ 0 };
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if ((t.oldLayout == vk::ImageLayout::eTransferDstOptimal || t.oldLayout == vk::ImageLayout::eTransferSrcOptimal) && t.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		//TODO: extract this as a property of the transfer that can be specified
		//TODO:::::::::::: fix this i'm not sure it this is right 
		destinationStage = vk::PipelineStageFlagBits::eAllCommands;//vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (t.oldLayout == vk::ImageLayout::eTransferDstOptimal && t.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		
		destinationStage = vk::PipelineStageFlagBits::eTransfer;//vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	//barrier.srcAccessMask = srcAccessMask; // TODO
	//barrier.dstAccessMask = dstAccessMask; // TODO

	cmdBuffer.pipelineBarrier(
		sourceStage, destinationStage, //TODO
		{},
		{},
		{},
		{ barrier });
}

void ResourceTransferer::performGenerateMipMapsTask(ResourceTransferer::GenerateMipMapsTask& t)
{
	ImageLayoutTransitionTask layoutTask;
	layoutTask.image = t.image;
	layoutTask.imageAspectMask = t.imageAspectMask;
	layoutTask.oldLayout = t.oldLayout;
	layoutTask.newLayout = vk::ImageLayout::eTransferSrcOptimal;


	int32_t mipWidth =  t.imageSize.width;
	int32_t mipHeight = t.imageSize.height;

	for (uint32_t i = 1; i < t.mipLevels; i++) {
		layoutTask.baseMipLevel = i - 1;

		performImageLayoutTransitionTask(layoutTask);
	
		vk::ImageBlit blit{};
		blit.srcOffsets[0] = vk::Offset3D( 0, 0, 0 );
		blit.srcOffsets[1] = vk::Offset3D( mipWidth, mipHeight, 1 );
		blit.srcSubresource.aspectMask = layoutTask.imageAspectMask;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = vk::Offset3D( 0, 0, 0 );
		blit.dstOffsets[1] = vk::Offset3D( mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 );
		blit.dstSubresource.aspectMask = layoutTask.imageAspectMask;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		cmdBuffer.blitImage(t.image, vk::ImageLayout::eTransferSrcOptimal, t.image, vk::ImageLayout::eTransferDstOptimal, { blit }, vk::Filter::eLinear);


		layoutTask.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		layoutTask.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		performImageLayoutTransitionTask(layoutTask);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}


	layoutTask.baseMipLevel = t.mipLevels - 1;
	layoutTask.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
	layoutTask.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

	performImageLayoutTransitionTask(layoutTask);



}
