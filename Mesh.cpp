#include "Mesh.h"


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
	auto bufferSize =
		baseMesh->verts.size() * sizeof(glm::vec3) + baseMesh->uvs.size() * sizeof(glm::vec2) +
		baseMesh->normals.size() * sizeof(glm::vec3) + baseMesh->tangents.size() * sizeof(glm::vec3) + baseMesh->bitangents.size() * sizeof(glm::vec3);

	buffer = new Buffer(device,allocator, (VkDeviceSize)bufferSize, options);
}

MeshBuffer::~MeshBuffer()
{
	delete buffer
}

void MeshBuffer::writeMeshToBuffer(bool mapandUnmap)
{
	if (mapandUnmap)
		buffer->mapMemory();

	memcpy(buffer->mappedData + 0, baseMesh->verts.data(), baseMesh->verts.size() * sizeof(glm::vec3));
	memcpy(buffer->mappedData + ,);
	memcpy(buffer->mappedData + , );
	memcpy(buffer->mappedData + baseMesh->verts.size() * sizeof(glm::vec3) + baseMesh->uvs.size() * sizeof(glm::vec2) + , );
	memcpy(buffer->mappedData + baseMesh->verts.size() * sizeof(glm::vec3) + baseMesh->uvs.size() * sizeof(glm::vec2) + baseMesh->normals.size() * sizeof(glm::vec3));
	
	if (mapandUnmap)
		buffer->unmapMemory();
}

void MeshBuffer::bindIntoCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t baseBinding)
{
	commandBuffer.bindVertexBuffers(baseBinding, {
			buffer->vkItem,
			buffer->vkItem,
			buffer->vkItem,
			buffer->vkItem,
			buffer->vkItem
		}, {
			0,
			baseMesh->verts.size() * sizeof(glm::vec3),
			baseMesh->verts.size() * sizeof(glm::vec3) + baseMesh->uvs.size() * sizeof(glm::vec2),
			baseMesh->verts.size() * sizeof(glm::vec3) + baseMesh->uvs.size() * sizeof(glm::vec2) + baseMesh->normals.size() * sizeof(glm::vec3),
			baseMesh->verts.size() * sizeof(glm::vec3) + baseMesh->uvs.size() * sizeof(glm::vec2) + baseMesh->normals.size() * sizeof(glm::vec3) + baseMesh->tangents.size() * sizeof(glm::vec3),
		});
}
