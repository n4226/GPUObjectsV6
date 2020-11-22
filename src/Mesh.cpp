#include "Mesh.h"
#include "pch.h"


std::array<VkVertexInputBindingDescription, 5> Mesh::getBindingDescription() {
	std::array<VkVertexInputBindingDescription, 5> bindingDescriptions = {
		makeVertBinding(0, sizeof(glm::vec3)),
		makeVertBinding(1, sizeof(glm::vec2)),
		makeVertBinding(2, sizeof(glm::vec3)),
		makeVertBinding(3, sizeof(glm::vec3)),
		makeVertBinding(4, sizeof(glm::vec3)),
	};


	return bindingDescriptions;
}

std::array<VkVertexInputAttributeDescription, 5> Mesh::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {
		makeVertAttribute(0,0,VertexAttributeFormat::vec3,0),
		makeVertAttribute(1,1,VertexAttributeFormat::vec2,0),
		makeVertAttribute(2,2,VertexAttributeFormat::vec3,0),
		makeVertAttribute(3,3,VertexAttributeFormat::vec3,0),
		makeVertAttribute(4,4,VertexAttributeFormat::vec3,0),
	};

	return attributeDescriptions;
}

VkDeviceSize Mesh::vertsSize()
{
	return verts.size() * sizeof(glm::vec3);
}

VkDeviceSize Mesh::uvsSize()
{
	return uvs.size() * sizeof(glm::vec2);
}

VkDeviceSize Mesh::normalsSize()
{
	return normals.size() * sizeof(glm::vec3);
}

VkDeviceSize Mesh::tangentsSize()
{
	return tangents.size() * sizeof(glm::vec3);
}

VkDeviceSize Mesh::bitangentsSize()
{
	return bitangents.size() * sizeof(glm::vec3);
}

VkDeviceSize Mesh::indiciesSize()
{
	return indicies.size() * sizeof(glm::uint32);
}

VkDeviceSize Mesh::vertsOffset()
{
	return 0;
}

VkDeviceSize Mesh::uvsOffset()
{
	return vertsOffset() + vertsSize();
}

VkDeviceSize Mesh::normalsOffset()
{
	return uvsOffset() + uvsSize();
}

VkDeviceSize Mesh::tangentsOffset()
{
	return normalsOffset() + normalsSize();
}

VkDeviceSize Mesh::bitangentsOffset()
{
	return tangentsOffset() + tangentsOffset();
}

VkDeviceSize Mesh::indiciesOffset()
{
	return bitangentsOffset() + bitangentsSize();
}

VkDeviceSize Mesh::fullSize()
{
	return indiciesOffset() + indiciesSize();
}

Mesh* Mesh::quad()
{
	Mesh* quad = new Mesh();

	quad->verts = {
		{-0.5f, -0.5f, 0.f},
		{0.5f, -0.5f, 0.f},
		{0.5f, 0.5f, 0.f},
		{-0.5f, 0.5f, 0.f},
	};

	quad->uvs = {
		{0,0},
		{1,0},
		{1,1},
		{0,1}
	};

	quad->normals = {
		{0,0.5,0},
		{0,0,0.5},
		{0.4,0.25,0},
		{0.5,0,0},
	};
	quad->tangents = {
		{0,0.5,0},
		{0,0,0.5},
		{0.4,0.25,0},
		{0.5,0,0},
	};
	quad->bitangents = {
		{0,0.5,0},
		{0,0,0.5},
		{0.4,0.25,0},
		{0.5,0,0},
	};

	quad->indicies = {
		0, 1, 2, 2, 3, 0
	};

	return quad;
}





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

/// <summary>
/// creates a buffer with the right size but does not populate it
/// </summary>
/// <param name="device"></param>
/// <param name="allocator"></param>
/// <param name="options"></param>
/// <param name="mesh"></param>
MeshBuffer::MeshBuffer(vk::Device device,VmaAllocator allocator, BufferCreationOptions options, Mesh* mesh)
	: baseMesh(mesh)
{
	auto bufferSize = baseMesh->fullSize();

	buffer = new Buffer(device,allocator, static_cast<VkDeviceSize>(bufferSize), options);
}

MeshBuffer::~MeshBuffer()
{
	delete buffer;
}

void MeshBuffer::writeMeshToBuffer(bool mapandUnmap)
{
	if (mapandUnmap)
		buffer->mapMemory();

	memcpy(static_cast<char*>(buffer->mappedData) +      baseMesh->vertsOffset(),      baseMesh->verts.data(),       baseMesh->vertsSize());
	memcpy(static_cast<char*>(buffer->mappedData) +        baseMesh->uvsOffset(),        baseMesh->uvs.data(),        baseMesh->uvsSize());
	memcpy(static_cast<char*>(buffer->mappedData) +    baseMesh->normalsOffset(),    baseMesh->normals.data(),    baseMesh->normalsSize());
	memcpy(static_cast<char*>(buffer->mappedData) +   baseMesh->tangentsOffset(),   baseMesh->tangents.data(),   baseMesh->tangentsSize());
	memcpy(static_cast<char*>(buffer->mappedData) + baseMesh->bitangentsOffset(), baseMesh->bitangents.data(), baseMesh->bitangentsSize());
	memcpy(static_cast<char*>(buffer->mappedData) +   baseMesh->indiciesOffset(),   baseMesh->indicies.data(),   baseMesh->indiciesSize());
	
	if (mapandUnmap)
		buffer->unmapMemory();
}

void MeshBuffer::bindVerticiesIntoCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t baseBinding)
{
	commandBuffer.bindVertexBuffers(baseBinding, {
			buffer->vkItem,
			buffer->vkItem,
			buffer->vkItem,
			buffer->vkItem,
			buffer->vkItem
		}, {
			baseMesh->vertsOffset(),
			baseMesh->uvsOffset(),
			baseMesh->normalsOffset(),
			baseMesh->tangentsOffset(),
			baseMesh->bitangentsOffset()
		});
}

void MeshBuffer::bindIndiciesIntoCommandBuffer(vk::CommandBuffer commandBuffer)
{
	commandBuffer.bindIndexBuffer(buffer->vkItem, baseMesh->indiciesOffset(), vk::IndexType::eUint32);
}
