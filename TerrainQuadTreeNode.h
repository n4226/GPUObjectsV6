#pragma once

#include "Box.h"
#include "glm/glm.hpp"

class TerrainQuadTreeNode
{
public:
	Box frame;
	glm::dvec3 center_geo;

};

