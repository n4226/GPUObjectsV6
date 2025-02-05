#include "pch.h"
#include "GPUGenCommandsPipeline.h"
//

#include "../../PipelineCreator.h"
#include "../../../../../Application/ApplicationRendererBridge/Application.h"
#include "../../../../dataObjects/Mesh.h"
#include "../../../../dataObjects/uniforms.h"

#include <vulkan/vulkan.hpp>
#include <array>


GPUGenCommandsPipeline::GPUGenCommandsPipeline(Application& app, vk::Device device, GraphicsPipeline& terrainPipeline)
{


	vk::DynamicLoader dl;
	//PFN_vkCreateIndirectCommandsLayoutNV fn = dl.getProcAddress<PFN_vkCreateIndirectCommandsLayoutNV>("vkCreateIndirectCommandsLayoutNV");


	// This dispatch class will fetch function pointers for the passed device if possible, else for the passed instance
	vk::DispatchLoaderDynamic dldid(app.instance, dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"), device);

	PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV = (PFN_vkCreateIndirectCommandsLayoutNV)dldid.vkGetDeviceProcAddr(device, "vkCreateIndirectCommandsLayoutNV");
	PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV = (PFN_vkGetGeneratedCommandsMemoryRequirementsNV)dldid.vkGetDeviceProcAddr(device, "vkGetGeneratedCommandsMemoryRequirementsNV");

	// Create VkIndirectCommandsLayoutNV

	{

		std::array<vk::IndirectCommandsLayoutTokenNV, 4> tokenLayouts{};
		//VkIndirectCommandsLayoutTokenNV
		tokenLayouts[0].tokenType = vk::IndirectCommandsTokenTypeNV::eDrawIndexed;
		tokenLayouts[0].stream = 0;
		tokenLayouts[0].offset = 0;
		auto types = vk::IndexType::eUint32;
		tokenLayouts[0].pIndexTypes = &types;
		tokenLayouts[0].indirectStateFlags = {};


		tokenLayouts[1].tokenType = vk::IndirectCommandsTokenTypeNV::eVertexBuffer;
		tokenLayouts[1].stream = 0;
		tokenLayouts[1].offset = 0;
		tokenLayouts[1].vertexBindingUnit = 0;
		tokenLayouts[1].vertexDynamicStride = VK_FALSE;

		tokenLayouts[2].tokenType = vk::IndirectCommandsTokenTypeNV::eIndexBuffer;
		tokenLayouts[2].stream = 0;
		tokenLayouts[2].offset = 0;

		tokenLayouts[3].tokenType = vk::IndirectCommandsTokenTypeNV::ePushConstant;
		tokenLayouts[3].stream = 0;
		tokenLayouts[3].offset = 0;
		tokenLayouts[3].pushconstantPipelineLayout = terrainPipeline.pipelineLayout;
		tokenLayouts[3].pushconstantShaderStageFlags = vk::ShaderStageFlagBits::eFragment;
		tokenLayouts[3].pushconstantOffset = 0;
		tokenLayouts[3].pushconstantSize = sizeof(DrawPushData);



		vk::IndirectCommandsLayoutCreateInfoNV info{};

		info.pNext = nullptr;
		info.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		
		// will be providoing a custom subset
		info.flags = vk::IndirectCommandsLayoutUsageFlagBitsNV::eIndexedSequences;
		info.setTokens(tokenLayouts);


		auto bindingDescription = Mesh::getBindingDescription();
		auto attributeDescriptions = Mesh::getAttributeDescriptions();

		std::vector<uint32_t> strides(bindingDescription.size());

		auto i = 0;
		for (size_t i = 0; i < bindingDescription.size(); i++)
		{
			strides[i] = bindingDescription[i].stride;
			i++;
		}

		info.streamCount = bindingDescription.size();
		info.setStreamStrides(strides);

		
		VkIndirectCommandsLayoutCreateInfoNV c_info = info;

		//vkCreateIndirectCommandsLayoutNV(device, &info_c, nullptr, result);

		

		vkCreateIndirectCommandsLayoutNV(device, &c_info, nullptr, &commandsLayout);

		//auto res = device.createIndirectCommandsLayoutNV(info,nullptr,dl);
	}

	//10.211.55.3
	// genrerate memory requirements

	{
		VkGeneratedCommandsMemoryRequirementsInfoNV info{};

		info.pipelineBindPoint = (VkPipelineBindPoint)vk::PipelineBindPoint::eGraphics;
		info.sType = VK_STRUCTURE_TYPE_GENERATED_COMMANDS_MEMORY_REQUIREMENTS_INFO_NV;
		info.pipeline = terrainPipeline.vkItem;
		info.indirectCommandsLayout = commandsLayout;
		info.maxSequencesCount = 10'000;

		VkMemoryRequirements2 result{};
		result.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		vkGetGeneratedCommandsMemoryRequirementsNV(device,&info,&result);

		BufferCreationOptions options =
		{ ResourceStorageType::gpu, { vk::BufferUsageFlagBits::eIndirectBuffer }, vk::SharingMode::eExclusive, {}};
		options.memoryTypeBits = result.memoryRequirements.memoryTypeBits;
		commandsBuffer = new Buffer(device,app.allocators[0],result.memoryRequirements.size,options);

	}

}
