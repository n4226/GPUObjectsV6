#pragma once

#include "../../dataObjects/Mesh.h"
#include "../../../pch.h"
#include "Math.h"
#include "glm/glm.hpp"
#include "TerrainQuadTreeNode.h"

class Renderer;
class TerrainSystem;

struct TreeNodeDrawResaourceToCoppy
{
	BinaryMeshSeirilizer* binMesh = nullptr;
	BinaryMeshAttrributes* binMeshAttributes = nullptr;
	Mesh* mesh = nullptr;
};

struct TreeNodeDrawData
{
	size_t vertIndex;
	size_t vertcount;
	std::vector<size_t> indIndicies;
	std::vector<size_t> indexCounts;
	size_t totalIndexCount;

	std::vector<DrawPushData> drawDatas;

	//buffer recipts
	BindlessMeshBuffer::WriteTransactionReceipt meshRecipt;
	BindlessMeshBuffer::WriteLocation modelRecipt;

	//CullInfo
	glm::vec3 aabbMin;
	glm::vec3 aabbMax;
};

class TerrainMeshLoader
{
public:
	Mesh* createChunkMesh(const TerrainQuadTreeNode& chunk);

private: 
	
	Renderer* renderer;
	TerrainSystem* terrainSystem;


	TreeNodeDrawResaourceToCoppy loadMeshPreDrawChunk(TerrainQuadTreeNode* node, bool inJob = false);
	void drawChunk(TerrainQuadTreeNode* node, TreeNodeDrawResaourceToCoppy preLoadedMesh, bool inJob = false);
	void removeDrawChunk(TerrainQuadTreeNode* node, bool inJob = false);


	friend TerrainSystem;
};

