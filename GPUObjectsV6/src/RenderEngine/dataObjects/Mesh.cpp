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
	return tangentsOffset() + tangentsSize();
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



BindlessMeshBuffer::BindlessMeshBuffer(vk::Device device, VmaAllocator allocator, BufferCreationOptions options, VkDeviceSize vCount, VkDeviceSize indexCount)
	: vCount(vCount), indexCount(indexCount)
{
	auto originalUsage = options.usage;

	auto vertbufferSize = (4 * sizeof(glm::vec3) + sizeof(glm::vec2)) * vCount;

	options.usage = originalUsage | vk::BufferUsageFlagBits::eVertexBuffer;

	vertBuffer = new Buffer(device, allocator, vertbufferSize, options);

	options.usage = originalUsage | vk::BufferUsageFlagBits::eIndexBuffer;

	indexBuffer = new Buffer(device, allocator, indexCount * sizeof(glm::uint32_t), options);

}

BindlessMeshBuffer::~BindlessMeshBuffer()
{
	delete vertBuffer;
	delete indexBuffer;
}

void BindlessMeshBuffer::writeMeshToBuffer(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, Mesh* mesh, bool mapandUnmap)
{
	PROFILE_FUNCTION
	if (mapandUnmap)
	{ 
		vertBuffer->mapMemory();
		indexBuffer->mapMemory();
	}

	memcpy(static_cast<char*>(vertBuffer->mappedData) + vertsOffset()      + vertIndex * sizeof(glm::vec3), mesh->verts.data(),      mesh->vertsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + uvsOffset()        + vertIndex * sizeof(glm::vec2), mesh->uvs.data(),        mesh->uvsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + normalsOffset()    + vertIndex * sizeof(glm::vec3), mesh->normals.data(),    mesh->normalsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + tangentsOffset()   + vertIndex * sizeof(glm::vec3), mesh->tangents.data(),   mesh->tangentsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + bitangentsOffset() + vertIndex * sizeof(glm::vec3), mesh->bitangents.data(), mesh->bitangentsSize());

	memcpy(static_cast<char*>(indexBuffer->mappedData)                     + indIndex * sizeof(glm::uint32), mesh->indicies.data(), mesh->indiciesSize());
	
	if (mapandUnmap)
	{
		vertBuffer->unmapMemory();
		indexBuffer->unmapMemory();
	}
}

BindlessMeshBuffer::WriteTransactionReceipt BindlessMeshBuffer::genrateWriteReceipt(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, Mesh* mesh)
{
	BindlessMeshBuffer::WriteTransactionReceipt report;

	BindlessMeshBuffer::WriteLocation location0 = { vertsOffset() + vertIndex * sizeof(glm::vec3), mesh->vertsSize()           };
	BindlessMeshBuffer::WriteLocation location1 = { uvsOffset() + vertIndex * sizeof(glm::vec2), mesh->uvsSize()               };
	BindlessMeshBuffer::WriteLocation location2 = { normalsOffset() + vertIndex * sizeof(glm::vec3), mesh->normalsSize()       };
	BindlessMeshBuffer::WriteLocation location3 = { tangentsOffset() + vertIndex * sizeof(glm::vec3), mesh->tangentsSize()     };
	BindlessMeshBuffer::WriteLocation location4 = { bitangentsOffset() + vertIndex * sizeof(glm::vec3), mesh->bitangentsSize() };
																												  
	report.vertexLocations = {
		location0,
		location1,
		location2,
		location3,
		location4,
	};
	report.indexLocation = { indIndex * sizeof(glm::uint32), mesh->indiciesSize() };
	return report;
}

void BindlessMeshBuffer::writeMeshToBuffer(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, BinaryMeshSeirilizer* mesh, bool mapandUnmap)
{
	PROFILE_FUNCTION

	if (mapandUnmap)
	{
		vertBuffer->mapMemory();
		indexBuffer->mapMemory();
	}

	memcpy(static_cast<char*>(vertBuffer->mappedData) + vertsOffset() + vertIndex * sizeof(glm::vec3)     , mesh->vertsPtr()      , mesh->vertsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + uvsOffset() + vertIndex * sizeof(glm::vec2)       , mesh->uvsPtr()        , mesh->uvsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + normalsOffset() + vertIndex * sizeof(glm::vec3)   , mesh->normalsPtr()    , mesh->normalsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + tangentsOffset() + vertIndex * sizeof(glm::vec3)  , mesh->tangentsPtr()   , mesh->tangentsSize());
	memcpy(static_cast<char*>(vertBuffer->mappedData) + bitangentsOffset() + vertIndex * sizeof(glm::vec3), mesh->bitangentsPtr() , mesh->bitangentsSize());

	memcpy(static_cast<char*>(indexBuffer->mappedData) + indIndex * sizeof(glm::uint32), mesh->indiciesPtr(0), mesh->AllSubMeshIndiciesSize());

	if (mapandUnmap)
	{
		vertBuffer->unmapMemory();
		indexBuffer->unmapMemory();
	}
}

BindlessMeshBuffer::WriteTransactionReceipt BindlessMeshBuffer::genrateWriteReceipt(VkDeviceAddress vertIndex, VkDeviceAddress indIndex, BinaryMeshSeirilizer* mesh)
{
	BindlessMeshBuffer::WriteTransactionReceipt report;

	BindlessMeshBuffer::WriteLocation location0 = { vertsOffset() + vertIndex * sizeof(glm::vec3), mesh->vertsSize() };
	BindlessMeshBuffer::WriteLocation location1 = { uvsOffset() + vertIndex * sizeof(glm::vec2), mesh->uvsSize() };
	BindlessMeshBuffer::WriteLocation location2 = { normalsOffset() + vertIndex * sizeof(glm::vec3), mesh->normalsSize() };
	BindlessMeshBuffer::WriteLocation location3 = { tangentsOffset() + vertIndex * sizeof(glm::vec3), mesh->tangentsSize() };
	BindlessMeshBuffer::WriteLocation location4 = { bitangentsOffset() + vertIndex * sizeof(glm::vec3), mesh->bitangentsSize() };

	report.vertexLocations = {
		location0,
		location1,
		location2,
		location3,
		location4,
	};
	report.indexLocation = { indIndex * sizeof(glm::uint32), mesh->AllSubMeshIndiciesSize() };
	return report;
}

void BindlessMeshBuffer::bindVerticiesIntoCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t baseBinding)
{
	commandBuffer.bindVertexBuffers(baseBinding, {
			vertBuffer->vkItem,
			vertBuffer->vkItem,
			vertBuffer->vkItem,
			vertBuffer->vkItem,
			vertBuffer->vkItem
		}, {
			vertsOffset(),
			uvsOffset(),
			normalsOffset(),
			tangentsOffset(),
			bitangentsOffset()
		});
}

void BindlessMeshBuffer::bindIndiciesIntoCommandBuffer(vk::CommandBuffer commandBuffer)
{
	commandBuffer.bindIndexBuffer(indexBuffer->vkItem,0, vk::IndexType::eUint32);
}

VkDeviceSize BindlessMeshBuffer::vertsSize()
{
	return vCount * sizeof(glm::vec3);
}

VkDeviceSize BindlessMeshBuffer::uvsSize()
{
	return vCount * sizeof(glm::vec2);
}

VkDeviceSize BindlessMeshBuffer::normalsSize()
{
	return vCount * sizeof(glm::vec3);
}

VkDeviceSize BindlessMeshBuffer::tangentsSize()
{
	return vCount * sizeof(glm::vec3);
}

VkDeviceSize BindlessMeshBuffer::bitangentsSize()
{
	return vCount * sizeof(glm::vec3);
}

VkDeviceSize BindlessMeshBuffer::indiciesSize()
{
	return indexCount * sizeof(glm::uint32);
}

VkDeviceSize BindlessMeshBuffer::vertsOffset()
{
	return 0;
}

VkDeviceSize BindlessMeshBuffer::uvsOffset()
{
	return vertsOffset() + vertsSize();
}

VkDeviceSize BindlessMeshBuffer::normalsOffset()
{
	return uvsOffset() + uvsSize();
}

VkDeviceSize BindlessMeshBuffer::tangentsOffset()
{
	return normalsOffset() + normalsSize();
}

VkDeviceSize BindlessMeshBuffer::bitangentsOffset()
{
	return tangentsOffset() + tangentsSize();
}
