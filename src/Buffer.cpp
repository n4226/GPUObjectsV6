#include "pch.h"
#include "Buffer.h"


// ALlocator = VMA
Buffer::Buffer(vk::Device device, VmaAllocator allocator, VkDeviceSize size, BufferCreationOptions options) // ResourceHeap* heap = nullptr
	: allocator(allocator), size(size), device(device)
{
	// create resource options
	
	vk::BufferCreateInfo createInfo;
	
	createInfo.setSize(size);
	createInfo.usage = options.usage;
	createInfo.sharingMode = options.sharingMode;

	//TODO: switch to use only c struct from the begginign to save perfromance 
	VkBufferCreateInfo cCreateInfo = createInfo;

	//vkItem = device.createBuffer(createInfo);
	


	// alloc memory options

	VmaAllocationCreateInfo allocInfo;

	allocInfo.flags = 0;//VMA_ALLOCATION_CREATE_MAPPED_BIT;
	allocInfo.preferredFlags = 0;
	allocInfo.requiredFlags = 0;
	allocInfo.memoryTypeBits = UINT32_MAX;
	allocInfo.pool = nullptr;
	allocInfo.pUserData = nullptr;
	allocInfo.usage = VmaMemoryUsage(options.storage);


	vmaCreateBuffer(allocator, &cCreateInfo, &allocInfo, &vkItem, &allocation, nullptr);

}

void Buffer::mapMemory()
{
	if (!memoryMapped) {
		vmaMapMemory(allocator, allocation, &mappedData);
		memoryMapped = true;
	}
}

void Buffer::unmapMemory()
{
	if (memoryMapped) {
		vmaUnmapMemory(allocator, allocation);
		memoryMapped = false;
	}
}

void Buffer::tempMapAndWrite(const void* data, size_t size)
{
	if (size == 0)
		size = this->size;
	mapMemory();
	memcpy(mappedData, data, (size_t)size);
	// not necicary on windows with big 3 drivers see vma docs on cash flushing for info
	//vmaFlushAllocation(allocator,stageVertBuffer.allocation,);
	unmapMemory();
}

bool Buffer::getMemoryMapped()
{
	return memoryMapped;
}

//TODO: fix this
bool Buffer::canMapMemory()
{
	return false;
}

void Buffer::gpuCopyToOther(Buffer& destination,vk::Queue& queue, vk::CommandPool commandPool)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer c_commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &c_commandBuffer);
	vk::CommandBuffer commandBuffer = c_commandBuffer;

	commandBuffer.begin({{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit }});

	gpuCopyToOther(destination, commandBuffer);

	commandBuffer.end();

	
	vk::SubmitInfo submitInfo{};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	queue.submit(submitInfo, nullptr);

	//TODO: add other options than waiting synchronusly for transfer

	queue.waitIdle();

	//TODO: fix conversion
	device.freeCommandBuffers(commandPool, commandBuffer);


}

void Buffer::gpuCopyToOther(Buffer& destination, vk::CommandBuffer& buffer)
{
	auto size = this->size;

	if (destination.size < size)
		size = destination.size;

	vk::BufferCopy regon(0,0,size);

	buffer.copyBuffer(vkItem, destination.vkItem, regon);

}


Buffer::~Buffer()
{
	//evice.destroyBuffer(vkItem);
	vmaDestroyBuffer(allocator, vkItem, allocation);
}


Buffer* Buffer::StageAndCreatePrivate(vk::Device device, vk::Queue& queue, vk::CommandPool commandPool, VmaAllocator allocator, VkDeviceSize size, const void* data, BufferCreationOptions options)
{

	auto orignalUsage = options.usage;

	options.usage |= vk::BufferUsageFlagBits::eTransferSrc;
	options.storage = BufferCreationOptions::cpu;

	Buffer staging = Buffer(device,allocator,size,options);

	staging.tempMapAndWrite(data,size);

	options.usage = orignalUsage | vk::BufferUsageFlagBits::eTransferDst;
	options.storage = BufferCreationOptions::gpu;

	Buffer* buffer = new Buffer(device, allocator, size, options);

	staging.gpuCopyToOther(*buffer, queue, commandPool);

	return buffer;
}