#include "pch.h"
#include "ResourceTransferTask.h"





MarlSafeTicketLock::MarlSafeTicketLock(marl::Ticket& ticket)
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
	auto ticket = ticketQueue.take();

	//marl::schedule([]() {printf("transfer job"); });
	
	if (synchronus)
		performTask(tasks, ticket, completionHandler);
	else
		marl::schedule([tasks, ticket, completionHandler, this]() { performTask(tasks, ticket, completionHandler); });
}


void ResourceTransferer::performTask(std::vector<Task> tasks, marl::Ticket ticket, std::function<void()> completionHandler)
{
	PROFILE_FUNCTION
	{
		//// will lock the ticket until the lock goes out of scope
		MarlSafeTicketLock lock(ticket);

		PROFILE_SCOPE("performTask Marl blocking")


		device.resetCommandPool(pool, {});

		cmdBuffer.begin({ { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } });

		// perform tasks
		for (Task& task : tasks) {
			cmdBuffer.copyBuffer(task.srcBuffer, task.dstBuffer, task.regions);
		}

		cmdBuffer.end();


		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		queue.submit(submitInfo, nullptr);

		//TODO: find better way than blocking the resource transfer marl queue

		queue.waitIdle();
	}
	// after ticket unlcok so other threads can access the vk resources
	//marl::blocking_call(completionHandler);
	completionHandler();
}