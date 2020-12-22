#pragma once
#include "../../../pch.h"

class Application;

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

	enum TaskType
	{
		bufferTransfers = 0,
		bufferToImageCopyWithTransition,
		imageLayoutTransition,
		generateMipMaps
	};

	struct BufferTransferTask {
		VkBuffer srcBuffer;
		VkBuffer dstBuffer;
		std::vector<vk::BufferCopy> regions;
	};

	struct ImageLayoutTransitionTask {
		vk::ImageLayout oldLayout;
		vk::ImageLayout newLayout;
		//vk::AccessFlags srcAccessMask;
		//vk::AccessFlags dstAccessMask;
		uint32_t baseMipLevel = 0;
		VkImage image;
		vk::ImageAspectFlags imageAspectMask;
	};

	struct BufferToImageCopyWithTransitionTask
	{
		vk::ImageLayout oldLayout;
		vk::ImageLayout finalLayout;
		
		VkBuffer buffer;
		VkImage image;
		vk::Extent3D imageSize;
		vk::ImageAspectFlags imageAspectMask;
	};

	struct GenerateMipMapsTask
	{
		VkImage image;
		vk::ImageLayout oldLayout;
		vk::ImageLayout finalLayout;

		uint32_t mipLevels;
		vk::Extent3D imageSize;
		vk::ImageAspectFlags imageAspectMask;
	};

	struct Task
	{
		//Task();
		TaskType type;

		BufferTransferTask bufferTransferTask;
		ImageLayoutTransitionTask imageLayoutTransitonTask;
		BufferToImageCopyWithTransitionTask bufferToImageCopyWithTransitionTask;
		GenerateMipMapsTask generateMipMapsTask;
	};

	//inline static ResourceTransferer* shared;

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
	friend Renderer;
	ResourceTransferer(vk::Device device, Renderer& renderer);
	~ResourceTransferer();
private:

	vk::CommandPool copyPool;
	vk::CommandBuffer cmdBuffer;


	
	vk::Device device;
	Renderer& renderer;
	
	/// <summary>
	/// marl sync primative
	/// </summary>
	marl::Ticket::Queue ticketQueue;

	/// <summary>
	/// exicuted on a marl thread
	/// </summary>
	/// <param name="ticket"></param>
	void performTask(std::vector<Task> tasks, marl::Ticket ticket, std::function<void()> completionHandler);

	void performBufferTransferTask(ResourceTransferer::BufferTransferTask& t);


	void performBufferToImageCopyWithTransitionTask(ResourceTransferer::BufferToImageCopyWithTransitionTask& t);
	void performImageLayoutTransitionTask(ResourceTransferer::ImageLayoutTransitionTask& t);
	void performGenerateMipMapsTask(ResourceTransferer::GenerateMipMapsTask& t);
};

