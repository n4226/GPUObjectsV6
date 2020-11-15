#pragma once

#include <vulkan/vulkan.hpp>
#include "ResourceHeap.h"

//#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

struct BufferCreationOptions {
	enum StorageType
	{
		gpu = VMA_MEMORY_USAGE_GPU_ONLY,
		cpu = VMA_MEMORY_USAGE_CPU_ONLY,
		cpuToGpu = VMA_MEMORY_USAGE_CPU_TO_GPU,
		gpuToCpu = VMA_MEMORY_USAGE_GPU_TO_CPU,
		cpuCopy = VMA_MEMORY_USAGE_CPU_COPY,
		/// <summary>
		/// Not available on non TBDR architecture GPUs
		/// </summary>
		gpuLazy = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED

	};
	StorageType storage;
	vk::BufferUsageFlags usage;
	vk::SharingMode sharingMode;
};

class Buffer
{
public:
	Buffer(const Buffer& othter) = delete;
	Buffer(vk::Device device, VmaAllocator allocator,VkDeviceSize size, BufferCreationOptions options);

	/// <summary>
	/// a staging buffer is created, the thread is blocked while the data is sent to the actual gpu private buffer, than the new [gpu private buffer is returned.
	/// </summary>
	/// <param name="device"></param>
	/// <param name="allocator"></param>
	/// <param name="size"></param>
	/// <param name="data"></param>
	/// <param name="options"></param>
	/// <returns></returns>
	static Buffer* StageAndCreatePrivate(vk::Device device, vk::Queue& queue, vk::CommandPool commandPool, VmaAllocator allocator, VkDeviceSize size,const void* data, BufferCreationOptions options);

	~Buffer();

	void mapMemory();
	void unmapMemory();

	void* mappedData = nullptr;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size">if left 0 the buffer size will be used</param>
	void tempMapAndWrite(const void* data,  size_t size = 0);

	bool getMemoryMapped();
	bool canMapMemory();

	VkBuffer vkItem = nullptr;
	VmaAllocation allocation = nullptr;
	const VkDeviceSize size;

	void gpuCopyToOther(Buffer& destination, vk::Queue& queue, vk::CommandPool commandPool);

	void gpuCopyToOther(Buffer& destination, vk::CommandBuffer& buffer);
	


private:
	bool memoryMapped = false;
	VmaAllocator allocator;
	vk::Device device;
};

