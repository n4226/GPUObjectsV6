#pragma once

#include "Math.h"
#include "TerrainQuadTree.h"
#include "Box.h"
#include "glm/glm.hpp"
#include <vector>



class TerrainQuadTreeNode
{
public:

	TerrainQuadTreeNode(Box frame,TerrainQuadTreeNode* parent,TerrainQuadTree* tree, uint16_t lodLevel = 0);

	Box frame;
	glm::dvec3 center_geo;
	glm::dvec3 start_geo;
	glm::dvec3 end_geo;

	TerrainQuadTreeNode* parent;
	bool isSplit = false;
	std::vector<TerrainQuadTreeNode> children;

	// stored properties 

	uint16_t lodLevel = 0;
	bool visable;
	bool active = true;

	TerrainQuadTree* tree;

	// render resources

	bool hasdraw = false;

	// lyfcycle events

	void split();
	void combine();

	void willBeCombined();

	bool operator<(const TerrainQuadTreeNode& rhs) const;
};

