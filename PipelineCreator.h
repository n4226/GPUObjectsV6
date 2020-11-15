#pragma once

#include "RenderPassManager.h"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <optional>
#include <fstream>
#include "Mesh.h"

class PipelineCreator
{
public:

	PipelineCreator(vk::Device device, vk::Extent2D swapChainExtent, RenderPassManager& renderPassManager);
	~PipelineCreator();

	static vk::PipelineShaderStageCreateInfo createShaderStageInfo(vk::Device device, const std::vector<char>& code,vk::ShaderStageFlagBits stage);

	static vk::ShaderModule createShaderModule(vk::Device device, const std::vector<char>& code);

	static std::vector<char> readFile(const std::string& filename);

	// pipeline properties

	vk::DescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;

	vk::Pipeline vkItem;

	RenderPassManager& renderPassManager;

	virtual void createGraphicsPipeline() = 0;
protected:

	vk::Device device;

	vk::Extent2D swapChainExtent;

};
