#pragma once

#include "Mesh.h"
#include "Box.h"
#include "Math.h"
#include "glm/glm.hpp"
#include "TerrainQuadTreeNode.h"

class TerrainMeshLoader
{

	Mesh* createChunkMesh(const TerrainQuadTreeNode& chunk,double radius);

};

