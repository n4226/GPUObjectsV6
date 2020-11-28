#pragma once

#include "pch.h"
#include "AttributeHelpers.h"
#include <glm/glm.hpp>
#include <vector>
#include "glm/glm.hpp"
#include "Buffer.h"
#include "rendering structures/BinaryMesh.h"


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

	// primatives

	static Mesh* quad();

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


class BindlessMeshBuffer {
public:

	struct WriteLocation
	{
		VkDeviceSize offset;
		VkDeviceSize size;
	};

	struct WriteTransactionReceipt
	{
		std::array<WriteLocation,5> vertexLocations;
		WriteLocation indexLocation;
	};

	BindlessMeshBuffer(vk::Device device, VmaAllocator allocator, BufferCreationOptions options,VkDeviceSize vCount, VkDeviceSize indexCount);
	~BindlessMeshBuffer();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mapandUnmap">if true will call map and unmap before and after writing</param>
	void writeMeshToBuffer(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, Mesh* mesh,bool mapandUnmap);
	WriteTransactionReceipt genrateWriteReceipt(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, Mesh* mesh);

	void writeMeshToBuffer(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, BinaryMeshSeirilizer* mesh, bool mapandUnmap);
	WriteTransactionReceipt genrateWriteReceipt(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, BinaryMeshSeirilizer* mesh);

	void bindVerticiesIntoCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t baseBinding);
	void bindIndiciesIntoCommandBuffer (vk::CommandBuffer commandBuffer);

	const VkDeviceSize vCount;
	const VkDeviceSize indexCount;

	Buffer* vertBuffer;
	Buffer* indexBuffer;

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

private:


};


// treianlge pipeline
struct TriangleVert {
	glm::vec3 position;
	glm::vec3 color;


	static VkVertexInputBindingDescription getBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};
