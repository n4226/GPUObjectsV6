#include "TerrainSystem.h"
#include "Renderer.h"

TerrainSystem::TerrainSystem(Renderer* renderer)
	: tree(Math::dEarthRad), renderer(renderer)
{
	PROFILE_FUNCTION

	CreateRenderResources();
}

void TerrainSystem::CreateRenderResources()
{

	vk::CommandPoolCreateInfo poolInfo{};

	poolInfo.queueFamilyIndex = renderer->window.queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = vk::CommandPoolCreateFlags(); // Optional

	pools.reserve(renderer->window.swapChainImageViews.size());
	commandBuffers.resize(renderer->window.swapChainImageViews.size());

	for (size_t i = 0; i < renderer->window.swapChainImageViews.size(); i++)
	{
		pools.push_back(renderer->device.createCommandPool(poolInfo));
		VkHelpers::allocateCommandBuffers(renderer->device, pools[i], commandBuffers.data() + i, 1,vk::CommandBufferLevel::ePrimary);
	}

}

void TerrainSystem::update()
{
	PROFILE_FUNCTION
	processTree();
}

void TerrainSystem::renderSystem(vk::CommandBuffer* buffers, uint32_t& count)
{
	PROFILE_FUNCTION

	
	
	
}

void TerrainSystem::processTree()
{
	return;
	PROFILE_FUNCTION

	auto adjustedOriginTrackedPos = static_cast<glm::dvec3>(trackedTransform->position) + *origin;

	for (TerrainQuadTreeNode* node: tree.leafNodes) {

		auto threshold = this->threshold(node);

		auto distance = glm::distance(adjustedOriginTrackedPos, node->center_geo);

		//TODO
		//... finish this

	}
}

void TerrainSystem::redrawChunk(const TerrainQuadTreeNode* node)
{

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
