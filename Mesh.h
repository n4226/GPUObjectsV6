#pragma once


#include "AttributeHelpers.h"
#include <glm/glm.hpp>
#include <vector>

struct Mesh
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	std::vector<uint32_t> indicies;
};

// treianlge pipeline
struct TriangleVert {
	glm::vec3 position;
	glm::vec3 color;


	static VkVertexInputBindingDescription getBindingDescription() {
		auto bindingDescription = makeVertBinding(0,sizeof(TriangleVert));
		


		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {
			makeVertAttribute(0,0,VertexAttributeFormat::vec3,offsetof(TriangleVert,position)),
			makeVertAttribute(0,1,VertexAttributeFormat::vec3,offsetof(TriangleVert,color)),
		};

		return attributeDescriptions;
	}
};
