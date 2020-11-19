#include "VkHelpers.h"

void VkHelpers::allocateCommandBuffers(vk::Device device, vk::CommandPool pool, vk::CommandBuffer* buffers, uint32_t count, vk::CommandBufferLevel level)
{
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.commandPool = pool;


	allocInfo.level = level;
	allocInfo.commandBufferCount = count;



	//auto result = vkAllocateCommandBuffers(device, &allocInfo, buffers);
	auto result = device.allocateCommandBuffers(&allocInfo,buffers);
}

