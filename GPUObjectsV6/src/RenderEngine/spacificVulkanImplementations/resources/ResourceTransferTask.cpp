#include "pch.h"
#include "ResourceTransferTask.h"





MarlSafeTicketLock::MarlSafeTicketLock(marl::Ticket ticket)
	: ticket(ticket)
{
	ticket.wait();
}

MarlSafeTicketLock::~MarlSafeTicketLock()
{
	ticket.done();
}



ResourceTransferer::ResourceTransferer(vk::Device device, vk::Queue queue, uint32_t queueFamilyIndex)
	: device(device), queue(queue), queueFamilyIndex(queueFamilyIndex), ticketQueue()
{

	vk::CommandPoolCreateInfo poolInfo{};

	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = vk::CommandPoolCreateFlags(); // Optional

	pool = device.createCommandPool(poolInfo);
	VkHelpers::allocateCommandBuffers(device, pool, &cmdBuffer, 1);
}

ResourceTransferer::~ResourceTransferer()
{
	ticketQueue.take().wait();
	device.destroyCommandPool(pool);
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


		device.resetCommandPool(pool, {});

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
			default:
				break;
			}

		}

		cmdBuffer.end();


		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		queue.submit(submitInfo, nullptr);

		//TODO: find better way than blocking the resource transfer marl queue

		queue.waitIdle();

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


	layoutTask.oldLayout = layoutTask.newLayout;
	layoutTask.newLayout = t.finalLayout;

	performImageLayoutTransitionTask(layoutTask);

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
	barrier.subresourceRange.baseMipLevel = 0;
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
	else if (t.oldLayout == vk::ImageLayout::eTransferDstOptimal && t.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		//TODO: extract this as a property of the transfer that can be specified
		//TODO:::::::::::: fix this i'm not sure it this is right 
		destinationStage = vk::PipelineStageFlagBits::eAllCommands;//vk::PipelineStageFlagBits::eFragmentShader;
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