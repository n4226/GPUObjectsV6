#pragma once

//#include "TerrainQuadTreeNode.h"
#include <vector>
#include "RenderSystem.h"

class TerrainQuadTreeNode;

class TerrainQuadTree
{
public:

	TerrainQuadTree(double radius);

	double radius;

	std::vector<TerrainQuadTreeNode> rootNodes;

	std::vector<TerrainQuadTreeNode*> leafNodes;


};

