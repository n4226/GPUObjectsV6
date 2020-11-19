#pragma once

#include "RenderSystem.h"
#include "TerrainQuadTree.h"
#include "TerrainQuadTreeNode.h"
#include "Transform.h"
#include "VkHelpers.h"

class Renderer;

class TerrainSystem: public RenderSystem
{
public:

	TerrainSystem(Renderer* renderer);

	void CreateRenderResources();

	void update() override;
	void renderSystem(vk::CommandBuffer* buffers, uint32_t& count) override;

	Transform* trackedTransform = nullptr;
	glm::dvec3* origin = nullptr;

	Renderer* renderer;

private:

	TerrainQuadTree tree;


	void processTree();

	void redrawChunk(const TerrainQuadTreeNode* node);

	double threshold(const TerrainQuadTreeNode* node);

	bool determinActive(const TerrainQuadTreeNode* node);

	void setActiveState(TerrainQuadTreeNode* node);

	// Render Resources

	/// <summary>
	/// one for each drawable
	/// </summary>
	std::vector<vk::CommandPool> pools;

	std::vector<vk::CommandBuffer> commandBuffers;

};

