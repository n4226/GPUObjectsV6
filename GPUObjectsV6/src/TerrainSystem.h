#pragma once

#include "pch.h"
#include "RenderSystem.h"
#include "TerrainQuadTree.h"
#include "TerrainQuadTreeNode.h"
#include "TerrainMeshLoader.h"
#include "Transform.h"
#include "VkHelpers.h"

class Renderer;

struct TreeNodeDrawData
{
	size_t vertIndex;
	size_t vertcount;
	size_t indIndex;
	size_t indexCount;

	DrawPushData drawData;
};

class TerrainSystem: public RenderSystem
{
public:

	TerrainSystem(Renderer* renderer, glm::dvec3* origin);
	~TerrainSystem();

	void CreateRenderResources();

	void update() override;
	vk::CommandBuffer* renderSystem(uint32_t subpass) override;

	Transform* trackedTransform = nullptr;
	glm::dvec3* origin = nullptr;

	Renderer* renderer;

	const uint16_t lodLevels = 13;

private:

	// temp resources

	std::set<TerrainQuadTreeNode*> toSplit = {};
	std::set<TerrainQuadTreeNode*> toCombine = {};
	std::map<TerrainQuadTreeNode*, TreeNodeDrawData> pendingDrawObjects;

	TerrainQuadTree tree;

	TerrainMeshLoader meshLoader;

	void processTree();

	void drawChunk(TerrainQuadTreeNode* node);
	void removeDrawChunk(TerrainQuadTreeNode* node);

	double threshold(const TerrainQuadTreeNode* node);

	bool determinActive(const TerrainQuadTreeNode* node);

	void setActiveState(TerrainQuadTreeNode* node);

	//updating descriptors

	void writePendingDrawOobjects();


	// Render Resources

	/// <summary>
	/// one for each drawable
	/// </summary>
	std::vector<vk::CommandPool> cmdBufferPools;
	std::vector<vk::CommandBuffer> commandBuffers;

	/*vk::DescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;*/

	std::map<TerrainQuadTreeNode*,TreeNodeDrawData> drawObjects;

};

