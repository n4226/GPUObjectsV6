#pragma once

#include "RenderSystem.h"
#include "TerrainQuadTree.h"
#include "TerrainQuadTreeNode.h"
#include "Transform.h"

class TerrainSystem: public RenderSystem
{
public:

	TerrainSystem();

	void update() override;
	void renderSystem(vk::CommandBuffer buffer) override;

	Transform* trackedTransform = nullptr;
	glm::dvec3* origin = nullptr;

private:

	TerrainQuadTree tree;

	void processTree();

	void redrawChunk(const TerrainQuadTreeNode* node);

	double threshold(const TerrainQuadTreeNode* node);

	bool determinActive(const TerrainQuadTreeNode* node);

	void setActiveState(TerrainQuadTreeNode* node);

};

