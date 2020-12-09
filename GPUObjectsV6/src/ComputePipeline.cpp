#include "pch.h"
#include "ComputePipeline.h"

ComputePipeline::ComputePipeline(vk::Device device, std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts, const std::vector<char>& shaderFilePath)
{

    VkPipelineShaderStageCreateInfo shaderStage =
        PipelineCreator::createShaderStageInfo(device, shaderFilePath, vk::ShaderStageFlagBits::eCompute);


    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    //pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size(); // Optional
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);


    vk::ComputePipelineCreateInfo pipelineCreatInfo;

    pipelineCreatInfo.layout = pipelineLayout;
    pipelineCreatInfo.stage = shaderStage;

    auto result = device.createComputePipeline(vk::PipelineCache(nullptr), pipelineCreatInfo);

    pipeline = result.value;



}
