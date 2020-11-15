#pragma once

#include <vulkan/vulkan.hpp>
#include "AttributeHelpers.h"
#include <glm/glm.hpp>
#include <vector>
#include "glm/glm.hpp"
#include "Buffer.h"

struct Mesh
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	std::vector<glm::uint32> indicies;

	static std::array<VkVertexInputBindingDescription, 5> getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();

	// offset and size functions

	VkDeviceSize         vertsSize();
	VkDeviceSize           uvsSize();
	VkDeviceSize       normalsSize();
	VkDeviceSize      tangentsSize();
	VkDeviceSize    bitangentsSize();
	VkDeviceSize      indiciesSize();

	VkDeviceSize       vertsOffset();
	VkDeviceSize         uvsOffset();
	VkDeviceSize     normalsOffset();
	VkDeviceSize    tangentsOffset();
	VkDeviceSize  bitangentsOffset();
	VkDeviceSize    indiciesOffset();

	VkDeviceSize    fullSize();


};

/// <summary>
/// with the current implimentation, all vertex attributes and indicies are stored in a single buffer.
/// </summary>
class MeshBuffer {
public:
	MeshBuffer(vk::Device device, VmaAllocator allocator, BufferCreationOptions options,Mesh* mesh);
	~MeshBuffer();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mapandUnmap">if true will call map and unmap before and after writing</param>
	void writeMeshToBuffer(bool mapandUnmap);
	void bindVerticiesIntoCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t baseBinding);
	void bindIndiciesIntoCommandBuffer(vk::CommandBuffer commandBuffer);

	Buffer* buffer;
	Mesh* baseMesh;

private:
};

// treianlge pipeline
struct TriangleVert {
	glm::vec3 position;
	glm::vec3 color;


	static VkVertexInputBindingDescription getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};
