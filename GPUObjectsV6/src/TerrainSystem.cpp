#include "pch.h"
#include "TerrainSystem.h"
#include "Renderer.h"

TerrainSystem::TerrainSystem(Renderer* renderer, glm::dvec3* origin)
	: tree(10000), renderer(renderer), meshLoader(), origin(origin)
{
	PROFILE_FUNCTION

	CreateRenderResources();


	for (TerrainQuadTreeNode* child : tree.leafNodes) {
		drawChunk(child);
	}

	writePendingDrawOobjects();
}

TerrainSystem::~TerrainSystem()
{
	for (auto pool : cmdBufferPools)
	{
		renderer->device.destroyCommandPool(pool);
	}
}

void TerrainSystem::CreateRenderResources()
{
	PROFILE_FUNCTION

	VkHelpers::createPoolsAndCommandBufffers
		(renderer->device, cmdBufferPools, commandBuffers, renderer->window.swapChainImageViews.size(), renderer->window.queueFamilyIndices.graphicsFamily.value(), vk::CommandBufferLevel::eSecondary);

}

void TerrainSystem::update()
{
	PROFILE_FUNCTION

	processTree();
	writePendingDrawOobjects();
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

	buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, renderer->window.pipelineCreator->vkItem);

	// setup descriptor and buffer bindings
	
	buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, renderer->window.pipelineCreator->pipelineLayout, 0, { renderer->descriptorSets[renderer->window.currentSurfaceIndex] }, {});
	
	renderer->globalMeshStagingBuffer->bindVerticiesIntoCommandBuffer(*buffer, 0);
	renderer->globalMeshStagingBuffer->bindIndiciesIntoCommandBuffer(*buffer);

	// encode draws


	for (auto it = drawObjects.begin(); it != drawObjects.end(); it++)
	{
		buffer->pushConstants(renderer->window.pipelineCreator->pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(DrawPushData), &it->second.drawData);
		buffer->drawIndexed(it->second.indexCount,1, it->second.indIndex, it->second.vertIndex,0);
	}

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

			toSplit.insert(node);

		}
		else if (node->parent != nullptr && node->parent->isSplit)
		{
			auto nextThreshold = this->threshold(node->parent);

			auto nextDistance = glm::distance(adjustedOriginTrackedPos, node->parent->center_geo);

			if (nextDistance > nextThreshold * 1.1)
			{
				//combine node
				if (toCombine.count(node->parent) == 0)
					toCombine.insert(node->parent);
			}
		}

	}

	for (TerrainQuadTreeNode* node : toSplit) {
		removeDrawChunk(node);

		node->split();
		std::cout << "nodes created with level: " << node->lodLevel + 1 << std::endl;
		for (TerrainQuadTreeNode* child : node->children) {
			drawChunk(child);
		}
	}

	for (TerrainQuadTreeNode* node : toCombine) {

		for (TerrainQuadTreeNode* child : node->children) {
			removeDrawChunk(child);
		}
		node->combine();

		drawChunk(node);
	}

	toSplit.clear();
	toCombine.clear();
}

void TerrainSystem::drawChunk(TerrainQuadTreeNode* node)
{
	PROFILE_FUNCTION

		//TODO - probablty more efficiant if all buffer writes were done after the draw objects were cxreatted all at once (in the write draw commands function) to allow single mapping and unmaopping and so on
		// this would mean that this funciton would have to crateda nother object with temporary object eg mesh and model trans to be writien at the end of the system update
	if (node->hasdraw) return;

	auto mesh = meshLoader.createChunkMesh(*node);

	// these indicies are in vert count space - meaning 1 = 1 vert not 1 byte
	auto vertIndex = renderer->gloablVertAllocator->alloc(mesh->verts.size());
	auto indIndex = renderer->gloablIndAllocator->alloc(mesh->indicies.size());

	renderer->globalMeshStagingBuffer->writeMeshToBuffer(vertIndex,indIndex, mesh, true);

	//renderer->globalMeshStaging->vertBuffer->gpuCopyToOther(renderer->globalMesh->vertBuffer)

	// model 

	Transform transform;
	transform.position = node->center_geo - *origin;
	transform.scale = glm::vec3{ 1,1,1 };
	transform.rotation = glm::identity<glm::quat>();

	ModelUniforms mtrans;
	mtrans.model = transform.matrix();



	auto modelIndex = renderer->globalModelBufferAllocator->alloc();
	auto modelAllocSize = renderer->globalModelBufferAllocator->allocSize;

	renderer->globalModelBufferStaging->tempMapAndWrite(&mtrans,modelIndex,modelAllocSize);

	TreeNodeDrawData drawData;
	drawData.indIndex = indIndex;
	drawData.vertIndex = vertIndex;
	drawData.vertcount = mesh->verts.size();
	drawData.indexCount = mesh->indicies.size();

	drawData.drawData.modelIndex = static_cast<glm::uint32>(modelIndex / modelAllocSize);
	
	node->hasdraw = true;
	pendingDrawObjects[node] = drawData;

}

void TerrainSystem::removeDrawChunk(TerrainQuadTreeNode* node)
{
	PROFILE_FUNCTION

		node->hasdraw = false;
	auto draw = drawObjects[node];
	drawObjects.erase(node);

	//TODO: deallocate buffers here 

}

double TerrainSystem::threshold(const TerrainQuadTreeNode* node)
{
	auto nodeRad = Math::llaDistance(node->frame.start, node->frame.getEnd(), tree.radius);
	//      return  radius / (node.lodLevel + 1).double * 1;
	return nodeRad * 1;
}

bool TerrainSystem::determinActive(const TerrainQuadTreeNode* node)
{
	return true;
}

void TerrainSystem::setActiveState(TerrainQuadTreeNode* node)
{
	//TODO: fix this
}

void TerrainSystem::writePendingDrawOobjects()
{

	// write model transform descriptors

	//VkWriteDescriptorSet modelUniformsDescriptorWrite{};
	//modelUniformsDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//modelUniformsDescriptorWrite.dstSet = renderer->descriptorSets[i];
	//modelUniformsDescriptorWrite.dstBinding = 1;
	//modelUniformsDescriptorWrite.dstArrayElement = 0;

	//modelUniformsDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//modelUniformsDescriptorWrite.descriptorCount = 1;

	//modelUniformsDescriptorWrite.pBufferInfo = &globalUniformBufferInfo;
	//modelUniformsDescriptorWrite.pImageInfo = nullptr; // Optional
	//modelUniformsDescriptorWrite.pTexelBufferView = nullptr; // Optional

	drawObjects.insert(pendingDrawObjects.begin(), pendingDrawObjects.end());
	pendingDrawObjects.clear();
}
