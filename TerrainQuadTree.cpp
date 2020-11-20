#include "pch.h"
#include "TerrainQuadTree.h"
#include "TerrainQuadTreeNode.h"


TerrainQuadTree::TerrainQuadTree(double radius)
	: radius(radius)
{
	glm::dvec2 rootTileSize(90, 90);
	rootNodes = {
			TerrainQuadTreeNode(Box(glm::dvec2(-90,-90),  rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(-90,-180), rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(-90,   0), rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(-90,  90), rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(  0,  90), rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(  0,   0), rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(  0, -90), rootTileSize), nullptr, this),
			TerrainQuadTreeNode(Box(glm::dvec2(  0,-180), rootTileSize), nullptr, this),
	};

	this->leafNodes.reserve(rootNodes.size());

	for (TerrainQuadTreeNode& node : rootNodes)
	{
		this->leafNodes.push_back(&node);
	}

}

