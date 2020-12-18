#pragma once
#include "../../../pch.h"

struct MarlSafeTicketLock {
	/// <summary>
	/// waits on the ticket
	/// </summary>
	/// <param name="ticket"></param>
	MarlSafeTicketLock(marl::Ticket ticket);
	
	/// marks the ticket as done
	~MarlSafeTicketLock();
private:
	marl::Ticket& ticket;
};


class ResourceTransferer
{
public:
	struct Task
	{
		VkBuffer srcBuffer;
		VkBuffer dstBuffer;
		std::vector<vk::BufferCopy> regions;

		// image suport comming soon
	};

	inline static ResourceTransferer* shared;

	/// <summary>
	/// asynchronously performs a number of transfer tasks on the dedicated gpu transfer queue. 
	/// all buffer objects must not be touched by calling thread until onDone is called.
	/// 
	/// IMPORTANT: it is assumed that the calling thread is bound to the marl schedular 
	/// 
	/// multiple calls to this function will result in syncronus behavure ie only one set of tasks can be active at once.
	/// any subsiquent calls must wait for previus calls to complete.
	/// </summary>
	/// <param name="task"></param>
	/// <param name="onDone">called on the job thread</param>
	void newTask(std::vector<Task>& tasks, std::function<void()> completionHandler,bool synchronus = false);

protected:
	friend WindowManager;
	ResourceTransferer(vk::Device device, vk::Queue queue, uint32_t queueFamilyIndex);
	~ResourceTransferer();
private:

	vk::CommandPool pool;
	vk::CommandBuffer cmdBuffer;


	/// <summary>
	/// will need to change for multi gpu
	/// </summary>
	vk::Device device;
	vk::Queue queue;
	uint32_t queueFamilyIndex;

	/// <summary>
	/// marl sync primative
	/// </summary>
	marl::Ticket::Queue ticketQueue;

	/// <summary>
	/// exicuted on a marl thread
	/// </summary>
	/// <param name="ticket"></param>
	void performTask(std::vector<Task> tasks, marl::Ticket ticket, std::function<void()> completionHandler);
};

