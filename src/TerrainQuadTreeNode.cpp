#include "pch.h"
#include "TerrainQuadTreeNode.h"



TerrainQuadTreeNode::TerrainQuadTreeNode(Box frame, TerrainQuadTreeNode* parent, TerrainQuadTree* tree, uint16_t lodLevel)
	: frame(frame),
	center_geo(Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, tree->radius)),
	start_geo(Math::LlatoGeo(glm::dvec3(frame.start, 0), {}, tree->radius)),
	end_geo(Math::LlatoGeo(glm::dvec3(frame.getEnd(), 0), {}, tree->radius)),
	parent(parent), tree(tree), lodLevel(lodLevel), children(), visable(frame.size.x < 30)
{
	
}

void TerrainQuadTreeNode::split()
{
	if (isSplit) return;
	isSplit = true;

	auto chldSize = frame.size / 2.0;

	children.reserve(4);

	children.emplace_back(Box(frame.start, chldSize),                                                  this, tree, lodLevel + 1);
	children.emplace_back(Box(frame.start + glm::dvec2(0, frame.size.y / 2), chldSize),                this, tree, lodLevel + 1);
	children.emplace_back(Box(frame.start + glm::dvec2(frame.size.x / 2, frame.size.y / 2), chldSize), this, tree, lodLevel + 1);
	children.emplace_back(Box(frame.start + glm::dvec2(frame.size.x / 2, 0), chldSize),                this, tree, lodLevel + 1);

	if (tree->leafNodes.count(this) > 0) {
		tree->leafNodes.erase(this);
		tree->leafNodes.insert(&children[0]);
		tree->leafNodes.insert(&children[1]);
		tree->leafNodes.insert(&children[2]);
		tree->leafNodes.insert(&children[3]);
	}
}

void TerrainQuadTreeNode::combine()
{
	if (!isSplit) return;
	isSplit = false;

	for (TerrainQuadTreeNode& child : children)
	{
		child.willBeCombined();
		tree->leafNodes.erase(&child);
	}
	tree->leafNodes.insert(this);

	children.clear();
}

void TerrainQuadTreeNode::willBeCombined()
{

}
