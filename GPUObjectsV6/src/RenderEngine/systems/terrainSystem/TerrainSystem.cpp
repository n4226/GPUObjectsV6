#include "pch.h"
#include "TerrainSystem.h"
#include "../renderSystems/Renderer.h"
#include <filesystem>

TerrainSystem::TerrainSystem(Renderer* renderer, glm::dvec3* origin)
	: tree(Math::dEarthRad), renderer(renderer), meshLoader(), origin(origin)
{
	PROFILE_FUNCTION

	meshLoader.renderer = renderer;
	meshLoader.terrainSystem = this;

	CreateRenderResources();


	for (TerrainQuadTreeNode* child : tree.leafNodes) {
		meshLoader.drawChunk(child, meshLoader.loadMeshPreDrawChunk(child), false);
	}

	//writePendingDrawOobjects();
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
#if RenderMode == RenderModeCPU2
	cmdBuffsUpToDate.resize(renderer->window.swapChainImageViews.size());

	for (auto val: cmdBuffsUpToDate) {
		val = true;
	}
#endif
}

void TerrainSystem::update()
{
	PROFILE_FUNCTION

	processTree();
}

vk::CommandBuffer* TerrainSystem::renderSystem(uint32_t subpass)
{
	PROFILE_FUNCTION

	uint32_t bufferIndex = renderer->window.currentSurfaceIndex;

		// if CPU mode 2 than only re encode commands for this surface's command buffer when changes occor otherwise just return the cmd buff for hte current surface
#if RenderMode == RenderModeCPU2
	{
		//TODO: posibly make this a try lock to improve performance
		auto cmdsValid = drawCommandsValid.lock();

		if (*cmdsValid == false) {
			for (auto val : cmdBuffsUpToDate) {
				val = false;
			}
		*cmdsValid = true;    
		}
		if (cmdBuffsUpToDate[bufferIndex] == true)
			return &commandBuffers[renderer->window.currentSurfaceIndex];
		else
			cmdBuffsUpToDate[bufferIndex] = true;
	}

#endif

	renderer->window.device.resetCommandPool(cmdBufferPools[bufferIndex], {});

	vk::CommandBuffer* buffer = &commandBuffers[renderer->window.currentSurfaceIndex];


	vk::CommandBufferInheritanceInfo inheritanceInfo{};
	inheritanceInfo.renderPass = renderer->window.renderPassManager->renderPass;
	inheritanceInfo.subpass = subpass;
	inheritanceInfo.framebuffer = renderer->window.swapChainFramebuffers[renderer->window.currentSurfaceIndex];

	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eRenderPassContinue; // Optional
	beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

	buffer->begin(beginInfo);

	buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, renderer->window.pipelineCreator->vkItem);

	// setup descriptor and buffer bindings

	buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, renderer->window.pipelineCreator->pipelineLayout, 0, { renderer->descriptorSets[renderer->window.currentSurfaceIndex] }, {});

	renderer->globalMeshBuffer->bindVerticiesIntoCommandBuffer(*buffer, 0);
	renderer->globalMeshBuffer->bindIndiciesIntoCommandBuffer(*buffer);

	// encode draws
	{

		auto drawObjects = this->drawObjects.lock();
		//printf("number of draws = %d \n", drawObjects->size());
		for (auto it = drawObjects->begin(); it != drawObjects->end(); it++)
		{

			//// frustrom cull
#if RenderMode == RenderModeCPU1
			if (!renderer->camFrustrom->IsBoxVisible(it->second.aabbMin, it->second.aabbMax)) {
				continue;
			}
#endif
			buffer->pushConstants(renderer->window.pipelineCreator->pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(DrawPushData), &it->second.drawData);
			buffer->drawIndexed(it->second.indexCount, 1, it->second.indIndex, it->second.vertIndex, 0);
		}
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
		auto shouldRunLoop = safeToModifyChunks.try_lock_shared();
		if (shouldRunLoop == nullptr || (*shouldRunLoop) == false) return;
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
		toDestroyDraw.insert(node);
		//meshLoader.removeDrawChunk(node);

		node->split();
		//std::cout << "nodes created with level: " << node->lodLevel + 1 << std::endl;
		for (TerrainQuadTreeNode* child : node->children) {
			toDrawDraw.insert(child);
			//drawChunk(child);
		}
	}

	for (TerrainQuadTreeNode* node : toCombine) {

		for (TerrainQuadTreeNode* child : node->children) {
			toDestroyDraw.insert(child);
			//meshLoader.removeDrawChunk(child);
			assert(child->children.size() == 0);
		}
		// will have to be combined after the draw is removed 
		//node->combine();

		toDrawDraw.insert(node);
		//drawChunk(node);
	}

	// draw in jobs

	if (toDrawDraw.size() > 0)
	{
		{
			auto shouldRunLoop = safeToModifyChunks.lock();
			*shouldRunLoop = false;
		}

			//auto ticket = ticketQueue.take();

			marl::schedule([this]() {
				PROFILE_SCOPE("create draw draw job")
					//MarlSafeTicketLock lock(ticket);

				marl::WaitGroup preLoadingWaitGroup(toDrawDraw.size());

				for (TerrainQuadTreeNode* chunk : toDrawDraw) {
					marl::schedule([this,chunk,preLoadingWaitGroup]() {
						defer(preLoadingWaitGroup.done());
						meshLoader.loadMeshPreDrawChunk(chunk, true);
					});
				}
				
				preLoadingWaitGroup.wait();

				for (TerrainQuadTreeNode* chunk : toDrawDraw) {
					meshLoader.drawChunk(chunk, {}, false);
				}

				writePendingDrawOobjects();

				for (TerrainQuadTreeNode* chunk : toDestroyDraw) {
					meshLoader.removeDrawChunk(chunk);
				}

				for (TerrainQuadTreeNode* chunk : toCombine) {
					chunk->combine();
				}

				toCombine.clear();
				toDestroyDraw.clear();
				toDrawDraw.clear();


				{
					auto shouldRunLoop = safeToModifyChunks.lock();
					*shouldRunLoop = true;
					destroyAwaitingNodes = true;
				}
#if RenderMode == RenderModeCPU2
				{
					auto cmdsValid = drawCommandsValid.lock();
					*cmdsValid = false;
				}
#endif
			});

	}
	else {
		//for (TerrainQuadTreeNode* chunk : toDestroyDraw) {
		//	meshLoader.removeDrawChunk(chunk);
		//}

		//for (TerrainQuadTreeNode* chunk : toCombine) {
		//	chunk->combine();
		//}

		//toCombine.clear();
		//toDestroyDraw.clear();
	}




	toSplit.clear();
	//toCombine.clear();

	//if (destroyAwaitingNodes) {
	//	for (TerrainQuadTreeNode* node : toDestroyDraw) {
	//		removeDrawChunk(node);
	//	}
	//	destroyAwaitingNodes = false;
	//}

	//writePendingDrawOobjects();
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

	{
		auto drawQueue = pendingDrawObjects.lock_shared();
		if (drawQueue->size() == 0) return;

		for (std::pair<TerrainQuadTreeNode*, TreeNodeDrawData> objectData : *drawQueue) {

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
	}
	
	

	std::vector<ResourceTransferer::Task> tasks = { vertexTask,indexTask,modelTask };

	ResourceTransferer::shared->newTask(tasks, [&]() {
		{
			PROFILE_SCOPE("terrain system ResourceTransferer::shared->newTask completion callback")
			{
				auto drawQueue = pendingDrawObjects.lock();
				auto drawObjects = this->drawObjects.lock();
				drawObjects->insert(drawQueue->begin(), drawQueue->end());
				drawQueue->clear();
				//using namespace std::chrono_literals;
				//std::this_thread::sleep_for(1s);
			}
		}
	},true);

}
