#pragma once

//#include "TerrainQuadTreeNode.h"
#include "pch.h"
#include "../renderSystems/RenderSystem.h"

class TerrainQuadTreeNode;

class TerrainQuadTree
{
public:

	TerrainQuadTree(double radius);
	~TerrainQuadTree();

	double radius;

	std::vector<TerrainQuadTreeNode*> rootNodes;

	std::set<TerrainQuadTreeNode*> leafNodes;


};

