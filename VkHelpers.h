#pragma once

#include <vulkan/vulkan.hpp>

namespace VkHelpers
{
	void allocateCommandBuffers(vk::Device device, vk::CommandPool pool, vk::CommandBuffer* buffers, uint32_t count, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
};

