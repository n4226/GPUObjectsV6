#pragma once

#include "RenderPassManager.h"
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <optional>
#include <fstream>

class PipelineCreator
{

public:

	PipelineCreator(vk::Device device, vk::Extent2D swapChainExtent, RenderPassManager& renderPassManager);
	~PipelineCreator();

	void createGraphicsPipelines();

	static vk::PipelineShaderStageCreateInfo createShaderStageInfo(vk::Device device, const std::vector<char>& code,vk::ShaderStageFlagBits stage);

	static vk::ShaderModule createShaderModule(vk::Device device, const std::vector<char>& code);

	static std::vector<char> readFile(const std::string& filename);


	VkPipelineLayout pipelineLayout;

	vk::Pipeline graphicsPipeline;

	RenderPassManager& renderPassManager;

private:

	vk::Device device;

	vk::Extent2D swapChainExtent;

};

