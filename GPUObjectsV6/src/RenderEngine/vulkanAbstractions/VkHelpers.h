#pragma once

#include "pch.h"
#include "Image.h"

namespace VkHelpers
{
	void allocateCommandBuffers(vk::Device device, vk::CommandPool pool, vk::CommandBuffer* buffers, uint32_t count, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

	/// <summary>
	/// create a number of command buffer pools and populate each one with a command buffer
	/// </summary>
	/// <param name="device"></param>
	/// <param name="pools">empty vector to output pools</param>
	/// <param name="buffers">empty vector to output buffers</param>
	/// <param name="count"></param>
	/// <param name="queueFamilyIndex"></param>
	/// <param name="level"></param>
	void createPoolsAndCommandBufffers(vk::Device device, std::vector<vk::CommandPool>& pools, std::vector<vk::CommandBuffer>& buffers, uint32_t count, uint32_t queueFamilyIndex, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

	vk::ImageView createImageView(vk::Device device, VkImage image, ImageViewCreationOptions options);

};

