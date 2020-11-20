#include "PipelineCreator.h"
#include "pch.h"

PipelineCreator::PipelineCreator(vk::Device device, vk::Extent2D swapChainExtent, RenderPassManager& renderPassManager)
    : renderPassManager(renderPassManager), swapChainExtent(swapChainExtent)
{
    this->device = device;

}

PipelineCreator::~PipelineCreator()
{
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyPipeline(vkItem);

    device.destroyDescriptorSetLayout(descriptorSetLayout);
}




/// <summary>
/// wrapes a shader binary data into a PipelineShaderStageCreateInfo struct
/// </summary>
/// <param name="device"></param>
/// <param name="code"></param>
/// <param name="stage"></param>
/// <returns>WARNING the returned value needs to be deinitilized when it is done being used with device.destroyShaderModule </returns>
vk::PipelineShaderStageCreateInfo PipelineCreator::createShaderStageInfo(vk::Device device, const std::vector<char>& code, vk::ShaderStageFlagBits stage)
{
    auto shaderModule = PipelineCreator::createShaderModule(device, code);

    vk::PipelineShaderStageCreateInfo shaderStageInfo{};

    shaderStageInfo.stage = stage;//vk::ShaderStageFlagBits::eVertex;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = "main";


    return shaderStageInfo;
}

vk::ShaderModule PipelineCreator::createShaderModule(vk::Device device, const std::vector<char>& code)
{
    vk::ShaderModuleCreateInfo createInfo{};

    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    auto shaderModule = device.createShaderModule(createInfo,nullptr);

    return shaderModule;
}

std::vector<char> PipelineCreator::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
