#pragma once

//#include "TerrainQuadTreeNode.h"
#include "pch.h"
#include "RenderSystem.h"

class TerrainQuadTreeNode;

class TerrainQuadTree
{
public:

	TerrainQuadTree(double radius);

	double radius;

	std::vector<TerrainQuadTreeNode> rootNodes;

	std::set<TerrainQuadTreeNode*> leafNodes;


};

