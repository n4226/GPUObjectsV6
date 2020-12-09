#pragma once

#include "pch.h"


class ComputePipeline
{
	
public:
	ComputePipeline(vk::Device device, std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts, const std::string& shaderFilePath);


	//std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
	vk::PipelineLayout pipelineLayout;


	vk::Pipeline pipeline;
};

