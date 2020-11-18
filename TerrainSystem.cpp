#include "TerrainSystem.h"

TerrainSystem::TerrainSystem()
	: tree(Math::dEarthRad)
{
	PROFILE_FUNCTION
}

void TerrainSystem::update()
{
	PROFILE_FUNCTION
	processTree();
}

void TerrainSystem::renderSystem(vk::CommandBuffer buffer)
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
