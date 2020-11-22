#include "VkHelpers.h"
#include "pch.h"

void VkHelpers::allocateCommandBuffers(vk::Device device, vk::CommandPool pool, vk::CommandBuffer* buffers, uint32_t count, vk::CommandBufferLevel level)
{
	

	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.commandPool = pool;


	allocInfo.level = level;
	allocInfo.commandBufferCount = count;



	//auto result = vkAllocateCommandBuffers(device, &allocInfo, buffers);
	auto result = device.allocateCommandBuffers(&allocInfo,buffers);
}

void VkHelpers::createPoolsAndCommandBufffers(vk::Device device, std::vector<vk::CommandPool>& pools, std::vector<vk::CommandBuffer>& buffers, uint32_t count, uint32_t queueFamilyIndex, vk::CommandBufferLevel level)
{
	PROFILE_FUNCTION

	vk::CommandPoolCreateInfo poolInfo{};

	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = vk::CommandPoolCreateFlags(); // Optional

	pools.reserve(count);
	buffers.resize(count);

	for (size_t i = 0; i < count; i++)
	{
		pools.push_back(device.createCommandPool(poolInfo));
		VkHelpers::allocateCommandBuffers(device, pools[i], buffers.data() + i, 1, level);
	}

}

