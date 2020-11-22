#include "pch.h"
#include "TerrainSystem.h"
#include "Renderer.h"

TerrainSystem::TerrainSystem(Renderer* renderer)
	: tree(1), renderer(renderer), loader()
{
	PROFILE_FUNCTION

	CreateRenderResources();
}

void TerrainSystem::CreateRenderResources()
{
	PROFILE_FUNCTION

	VkHelpers::createPoolsAndCommandBufffers
		(renderer->device, cmdBufferPools, commandBuffers, renderer->window.swapChainImageViews.size(), renderer->window.queueFamilyIndices.graphicsFamily.value(), vk::CommandBufferLevel::eSecondary);

	vk::DescriptorPoolSize poolSize{};
	poolSize.type = vk::DescriptorType::eUniformBuffer;
	poolSize.descriptorCount = 1;

	vk::DescriptorPoolCreateInfo poolInfo{};
	poolInfo.setPoolSizes({ 1, &poolSize });

	poolInfo.maxSets = static_cast<uint32_t>(renderer->window.swapChainImageViews.size());

	descriptorPool = renderer->device.createDescriptorPool(poolInfo);

	std::vector<vk::DescriptorSetLayout> layouts(renderer->window.swapChainImages.size(), renderer->window.pipelineCreator->descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo{};
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(renderer->window.swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	VkDescriptorSetAllocateInfo c_allocInfo = allocInfo;

	descriptorSets.resize(renderer->window.swapChainImages.size());

	vkAllocateDescriptorSets(renderer->device, &c_allocInfo, descriptorSets.data());

	for (size_t i = 0; i < renderer->window.swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = renderer->uniformBuffers[i]->vkItem;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(TriangleUniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;

		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		
		renderer->device.updateDescriptorSets({ descriptorWrite }, {});
	}
}

void TerrainSystem::update()
{
	PROFILE_FUNCTION

	processTree();
}

vk::CommandBuffer* TerrainSystem::renderSystem(uint32_t subpass)
{
	PROFILE_FUNCTION

	renderer->window.device.resetCommandPool(cmdBufferPools[renderer->window.currentSurfaceIndex], {});

	vk::CommandBuffer* buffer = &commandBuffers[renderer->window.currentSurfaceIndex];
	

	vk::CommandBufferInheritanceInfo inheritanceInfo{};
	inheritanceInfo.renderPass =  renderer->window.renderPassManager->renderPass;
	inheritanceInfo.subpass = subpass;
	inheritanceInfo.framebuffer = renderer->window.swapChainFramebuffers[renderer->window.currentSurfaceIndex];

	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eRenderPassContinue; // Optional
	beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

	buffer->begin(beginInfo);

	// setup descriptor bindings
	
	buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, renderer->window.pipelineCreator->pipelineLayout, 0, { descriptorSets[renderer->window.currentSurfaceIndex] }, {});

	// encode draws

	static auto chunk = TerrainQuadTreeNode(Box(glm::dvec2(0, 0), glm::dvec2(45, 45)), nullptr, &tree);
	static auto mesh = loader.createChunkMesh(chunk);
	BufferCreationOptions bOptions = { BufferCreationOptions::cpuToGpu,{vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer}, vk::SharingMode::eExclusive };
	static auto meshBuff = new MeshBuffer(renderer->device, renderer->allocator, bOptions, mesh);
	meshBuff->writeMeshToBuffer(true);

	

	buffer->end();

	return buffer;
}

void TerrainSystem::processTree()
{
	PROFILE_FUNCTION

	auto adjustedOriginTrackedPos = static_cast<glm::dvec3>(trackedTransform->position) + *origin;

	for (TerrainQuadTreeNode* node: tree.leafNodes) {

		auto threshold = this->threshold(node);

		auto distance = glm::distance(adjustedOriginTrackedPos, node->center_geo);

		if (node->active && !node->isSplit && node->lodLevel < (lodLevels - 1) && distance < threshold) 
		{
			// split node

			removeDrawChunk(node);

			node->split();

			for (TerrainQuadTreeNode& child : node->children) {
				drawChunk(&child);
			}
		}
		else if (node->parent != nullptr && node->parent->isSplit)
		{
			auto nextThreshold = this->threshold(node->parent);

			auto nextDistance = glm::distance(adjustedOriginTrackedPos, node->parent->center_geo);

			if (nextDistance > nextThreshold * 1.1)
			{
				//combine node
				for (TerrainQuadTreeNode& child : node->parent->children) {
					removeDrawChunk(&child);
				}
				node->parent->combine();

				drawChunk(node->parent);
			}
		}

	}
}

void TerrainSystem::drawChunk(const TerrainQuadTreeNode* node)
{
	PROFILE_FUNCTION

	

}

void TerrainSystem::removeDrawChunk(const TerrainQuadTreeNode* node)
{
	PROFILE_FUNCTION


}

double TerrainSystem::threshold(const TerrainQuadTreeNode* node)
{
	auto nodeRad = Math::llaDistance(node->frame.start, node->frame.getEnd(), tree.radius);
	//      return  radius / (node.lodLevel + 1).double * 1;
	return nodeRad * 1;
}

bool TerrainSystem::determinActive(const TerrainQuadTreeNode* node)
{
	return false;
}

void TerrainSystem::setActiveState(TerrainQuadTreeNode* node)
{
	//TODO: fix this
}
