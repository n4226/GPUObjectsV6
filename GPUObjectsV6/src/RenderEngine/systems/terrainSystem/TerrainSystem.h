#pragma once

#include "pch.h"
#include "../renderSystems/RenderSystem.h"
#include "TerrainQuadTree.h"
#include "TerrainQuadTreeNode.h"
#include "TerrainMeshLoader.h"
#include "../../dataObjects/Transform.h"
#include "../../vulkanAbstractions/VkHelpers.h"

class Renderer;
class FloatingOriginSystem;



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

	double getRadius();

private:

	// temp resources

	std::set<TerrainQuadTreeNode*> toSplit = {};
	std::set<TerrainQuadTreeNode*> toCombine = {};
	std::set<TerrainQuadTreeNode*> toDestroyDraw = {};
	std::set<TerrainQuadTreeNode*> toDrawDraw = {};

	libguarded::shared_guarded<std::unordered_map<TerrainQuadTreeNode*,TreeNodeDrawResaourceToCoppy>> loadedMeshesToDraw = {};

	bool destroyAwaitingNodes = false;
	libguarded::shared_guarded<bool> safeToModifyChunks = libguarded::shared_guarded<bool>(true);

#if RenderMode == RenderModeCPU2
	libguarded::shared_guarded<bool> drawCommandsValid = libguarded::shared_guarded<bool>(false);
	std::vector<bool> cmdBuffsUpToDate;
#endif
	libguarded::shared_guarded<std::map<TerrainQuadTreeNode*, TreeNodeDrawData>> pendingDrawObjects;

	TerrainQuadTree tree;

	TerrainMeshLoader meshLoader;

	void processTree();


	double threshold(const TerrainQuadTreeNode* node);

	bool determinActive(const TerrainQuadTreeNode* node);

	void setActiveState(TerrainQuadTreeNode* node);

	//updating descriptors

	void writePendingDrawOobjects();

	//async resources
	marl::Ticket::Queue ticketQueue;

	// Render Resources

	/// <summary>
	/// one for each drawable
	/// </summary>
	std::vector<vk::CommandPool> cmdBufferPools;
	std::vector<vk::CommandBuffer> commandBuffers;

	/*vk::DescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;*/

	libguarded::plain_guarded<std::map<TerrainQuadTreeNode*,TreeNodeDrawData>> drawObjects;

	friend FloatingOriginSystem;
	friend TerrainMeshLoader;
};

