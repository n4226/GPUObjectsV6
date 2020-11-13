#include "Mesh.h"

VkVertexInputBindingDescription TriangleVert::getBindingDescription() {
	auto bindingDescription = makeVertBinding(0, sizeof(TriangleVert));



	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> TriangleVert::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {
		makeVertAttribute(0,0,VertexAttributeFormat::vec3,offsetof(TriangleVert,position)),
		makeVertAttribute(0,1,VertexAttributeFormat::vec3,offsetof(TriangleVert,color)),
	};

	return attributeDescriptions;
}
