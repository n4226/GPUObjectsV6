#pragma once

#include "pch.h"

namespace VkHelpers
{
	void allocateCommandBuffers(vk::Device device, vk::CommandPool pool, vk::CommandBuffer* buffers, uint32_t count, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

	//void createPoolsAndCommandBufffers(vk::Device device, uint32_t count, uint32_t queueFamilyIndex, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
};

