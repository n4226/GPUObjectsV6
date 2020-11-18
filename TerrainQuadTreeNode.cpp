#include "TerrainQuadTreeNode.h"




TerrainQuadTreeNode::TerrainQuadTreeNode(Box frame, TerrainQuadTreeNode* parent, TerrainQuadTree* tree)
	: frame(frame), 
	center_geo(Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, tree->radius)),
	start_geo(Math::LlatoGeo(glm::dvec3(frame.start, 0), {}, tree->radius)),
	end_geo(Math::LlatoGeo(glm::dvec3(frame.getEnd(), 0), {}, tree->radius)),
	parent(parent), tree(tree), children()
{

}
