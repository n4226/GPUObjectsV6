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

	void createGraphicsPipelines();

	void createTrianglePipeline();
	void createTerrainPipeline();

	static vk::PipelineShaderStageCreateInfo createShaderStageInfo(vk::Device device, const std::vector<char>& code,vk::ShaderStageFlagBits stage);

	static vk::ShaderModule createShaderModule(vk::Device device, const std::vector<char>& code);

	static std::vector<char> readFile(const std::string& filename);

	// pipelines

	vk::DescriptorSetLayout terrianDescriptorSetLayout;
	VkPipelineLayout terrianPipelineLayout;

	vk::DescriptorSetLayout triangleDescriptorSetLayout;
	VkPipelineLayout trianglePipelineLayout;


	vk::Pipeline trianglePipeline;
	vk::Pipeline pbrPipeline;
	vk::Pipeline terrianPipeline;

	RenderPassManager& renderPassManager;

private:

	vk::Device device;

	vk::Extent2D swapChainExtent;

};
