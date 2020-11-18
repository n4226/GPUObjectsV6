#pragma once

#include "Math.h"
#include "TerrainQuadTree.h"
#include "Box.h"
#include "glm/glm.hpp"
#include <vector>

class TerrainQuadTreeNode
{
public:

	TerrainQuadTreeNode(Box frame,TerrainQuadTreeNode* parent,TerrainQuadTree* tree);

	Box frame;
	glm::dvec3 center_geo;
	glm::dvec3 start_geo;
	glm::dvec3 end_geo;

	TerrainQuadTreeNode* parent;
	std::vector<TerrainQuadTreeNode*> children;


	TerrainQuadTree* tree;

};

