#pragma once

#include "Mesh.h"
#include "pch.h"
#include "Math.h"
#include "glm/glm.hpp"
#include "TerrainQuadTreeNode.h"

class TerrainMeshLoader
{
public:
	Mesh* createChunkMesh(const TerrainQuadTreeNode& chunk);


};

