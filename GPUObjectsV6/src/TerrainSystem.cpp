#include "pch.h"
#include "TerrainSystem.h"
#include "Renderer.h"
#include <filesystem>

TerrainSystem::TerrainSystem(Renderer* renderer, glm::dvec3* origin)
	: tree(Math::dEarthRad * 0 + 1'000), renderer(renderer), meshLoader(), origin(origin)
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

double TerrainSystem::getRadius()
{
	return tree.radius;
}

void TerrainSystem::processTree()
{
	PROFILE_FUNCTION

	{
		// his is temporary to prevent multiple sets of staging buffer coppies to rech deadlock currently
		auto shouldRunLoop = safeToModifyChunks.try_lock_shared().get();
		if (shouldRunLoop != nullptr && *shouldRunLoop == false) return;
	}

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

			if (nextDistance > (nextThreshold * 1.05))
			{
				//combine node

				auto split = true;

				for (TerrainQuadTreeNode* child : node->parent->children) {
					if (toSplit.count(child) > 0 || child->isSplit)
					{
						split = false;
						break;
					}
				}
				if (split)
					toCombine.insert(node->parent);
			}
		}

	}

	for (TerrainQuadTreeNode* node : toSplit) {
		//toDestroyDraw.insert(node);
		removeDrawChunk(node);

		node->split();
		//std::cout << "nodes created with level: " << node->lodLevel + 1 << std::endl;
		for (TerrainQuadTreeNode* child : node->children) {
			drawChunk(child);
		}
	}

	for (TerrainQuadTreeNode* node : toCombine) {

		for (TerrainQuadTreeNode* child : node->children) {
			//toDestroyDraw.insert(child);
			removeDrawChunk(child);
			assert(child->children.size() == 0);
		}
		node->combine();

		drawChunk(node);
	}

	toSplit.clear();
	toCombine.clear();

	//if (destroyAwaitingNodes) {
	//	for (TerrainQuadTreeNode* node : toDestroyDraw) {
	//		removeDrawChunk(node);
	//	}
	//	destroyAwaitingNodes = false;
	//}

	writePendingDrawOobjects();
}

void TerrainSystem::drawChunk(TerrainQuadTreeNode* node)
{
	PROFILE_FUNCTION

		//TODO - probablty more efficiant if all buffer writes were done after the draw objects were cxreatted all at once (in the write draw commands function) to allow single mapping and unmaopping and so on
		// this would mean that this funciton would have to crateda nother object with temporary object eg mesh and model trans to be writien at the end of the system update
	if (node->hasdraw) return;

	BindlessMeshBuffer::WriteTransactionReceipt meshReceipt;

	VkDeviceAddress vertIndex;
	VkDeviceAddress indIndex;
	size_t vertCount;
	size_t indCount;

	// get  and encode mesh
	//TODO store this in a better place
	const char* Terrain_Chunk_Mesh_Dir = R"(terrain/chunkMeshes/)";

	auto file = Terrain_Chunk_Mesh_Dir + node->frame.toString() + ".bmesh";
	if (std::filesystem::exists(file)) {
		auto mesh = BinaryMeshSeirilizer(file.c_str());

		vertCount = *mesh.vertCount;
		indCount = mesh.subMeshIndexCounts[0];

		// these indicies are in vert count space - meaning 1 = 1 vert not 1 byte
		vertIndex = renderer->gloablVertAllocator->alloc(vertCount);
		indIndex = renderer->gloablIndAllocator->alloc(indCount);

		renderer->globalMeshStagingBuffer->writeMeshToBuffer(vertIndex, indIndex, &mesh, true);


		//renderer->globalMeshStaging->vertBuffer->gpuCopyToOther(renderer->globalMesh->vertBuffer)
		meshReceipt = renderer->globalMeshStagingBuffer->genrateWriteReceipt(vertIndex, indIndex, &mesh);
	}
	else 
	{
		auto mesh = meshLoader.createChunkMesh(*node);

		vertCount = mesh->verts.size();
		indCount  = mesh->indicies.size();

		// these indicies are in vert count space - meaning 1 = 1 vert not 1 byte
		vertIndex = renderer->gloablVertAllocator->alloc(mesh->verts.size());
		indIndex = renderer->gloablIndAllocator->alloc(mesh->indicies.size());

		renderer->globalMeshStagingBuffer->writeMeshToBuffer(vertIndex, indIndex, mesh, true);


		//renderer->globalMeshStaging->vertBuffer->gpuCopyToOther(renderer->globalMesh->vertBuffer)
		meshReceipt = renderer->globalMeshStagingBuffer->genrateWriteReceipt(vertIndex, indIndex, mesh);
	}

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
	drawData.vertcount = vertCount;
	drawData.indexCount = indCount;
	drawData.meshRecipt = meshReceipt;
	drawData.modelRecipt = { modelIndex,modelAllocSize };

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
	return nodeRad * 0.9;
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
	if (pendingDrawObjects.size() == 0) return;
	// copy from staging buffers to gpu ones - asynchronously

	ResourceTransferer::Task vertexTask;

	vertexTask.srcBuffer = renderer->globalMeshStagingBuffer->vertBuffer->vkItem;
	vertexTask.dstBuffer = renderer->globalMeshBuffer->vertBuffer->vkItem;


	ResourceTransferer::Task indexTask;

	indexTask.srcBuffer = renderer->globalMeshStagingBuffer->indexBuffer->vkItem;
	indexTask.dstBuffer = renderer->globalMeshBuffer->indexBuffer->vkItem;



	ResourceTransferer::Task modelTask;

	modelTask.srcBuffer = renderer->globalModelBufferStaging->vkItem;
	modelTask.dstBuffer = renderer->globalModelBuffers[renderer->gpuActiveGlobalModelBuffer]->vkItem;


	for (std::pair<TerrainQuadTreeNode*,TreeNodeDrawData> objectData : pendingDrawObjects) {

		for (size_t i = 0; i < objectData.second.meshRecipt.vertexLocations.size(); i++)
		{
			BindlessMeshBuffer::WriteLocation& vertRecpt = objectData.second.meshRecipt.vertexLocations[i];
			vertexTask.regions.emplace_back(vertRecpt.offset, vertRecpt.offset, vertRecpt.size);
		}

		BindlessMeshBuffer::WriteLocation& meshRecpt = objectData.second.meshRecipt.indexLocation;
		indexTask.regions.emplace_back(meshRecpt.offset, meshRecpt.offset, meshRecpt.size);

		BindlessMeshBuffer::WriteLocation& modelRecpt = objectData.second.modelRecipt;
		modelTask.regions.emplace_back(modelRecpt.offset, modelRecpt.offset, modelRecpt.size);
	}

	{
		auto shouldRunLoop = safeToModifyChunks.lock().get();
		*shouldRunLoop = false;
	}
	

	std::vector<ResourceTransferer::Task> tasks = { vertexTask,indexTask,modelTask };

	ResourceTransferer::shared->newTask(tasks, [&]() {
		{
			PROFILE_SCOPE("terrain system ResourceTransferer::shared->newTask completion callback")
			drawObjects.insert(pendingDrawObjects.begin(), pendingDrawObjects.end());
			pendingDrawObjects.clear();
			{
				auto shouldRunLoop = safeToModifyChunks.lock().get();
				*shouldRunLoop = true;
				destroyAwaitingNodes = true;
			}
		}
	},true);

}
