#include "BinaryMesh.h"
#include <assert.h>
#include <numeric>

size_t BinaryMeshSeirilizer::Mesh::vertsSize()
{
	return verts.size() * sizeof(glm::vec3);
}

size_t BinaryMeshSeirilizer::Mesh::uvsSize()
{
	return uvs.size() * sizeof(glm::vec2);
}

size_t BinaryMeshSeirilizer::Mesh::normalsSize()
{
	return normals.size() * sizeof(glm::vec3);
}

size_t BinaryMeshSeirilizer::Mesh::tangentsSize()
{
	return tangents.size() * sizeof(glm::vec3);
}

size_t BinaryMeshSeirilizer::Mesh::bitangentsSize()
{
	return bitangents.size() * sizeof(glm::vec3);
}

size_t BinaryMeshSeirilizer::Mesh::indiciesSize()
{
	return indicies.size() * sizeof(glm::uint32);
}

size_t BinaryMeshSeirilizer::Mesh::vertsOffset()
{
	return 0;
}

size_t BinaryMeshSeirilizer::Mesh::uvsOffset()
{
	return vertsOffset() + vertsSize();
}

size_t BinaryMeshSeirilizer::Mesh::normalsOffset()
{
	return uvsOffset() + uvsSize();
}

size_t BinaryMeshSeirilizer::Mesh::tangentsOffset()
{
	return normalsOffset() + normalsSize();
}

size_t BinaryMeshSeirilizer::Mesh::bitangentsOffset()
{
	return tangentsOffset() + tangentsSize();
}

size_t BinaryMeshSeirilizer::Mesh::indiciesOffset()
{
	return bitangentsOffset() + bitangentsSize();
}

size_t BinaryMeshSeirilizer::Mesh::fullSize()
{
	return indiciesOffset() + indiciesSize();
}

BinaryMeshSeirilizer::BinaryMeshSeirilizer(Mesh& originalMesh)
{
	assert(originalMesh.verts.size() == originalMesh.uvs.size() == originalMesh.normals.size() == originalMesh.tangents.size() == originalMesh.bitangents.size());

	auto headerLength = sizeof(uint32_t) * (2 + originalMesh.indicies.size());
	auto verticiesLength = originalMesh.indiciesOffset();
	auto indiciesLength = 0;

	for (std::vector<uint32_t>& subMesh : originalMesh.indicies) {
		indiciesLength += subMesh.size();
	}

	size_t meshLength = headerLength + verticiesLength + indiciesLength;

	this->meshLength = meshLength;
	mesh = malloc(meshLength);

	//populate the data

	//header
	vertCount = (reinterpret_cast<uint32_t*>(mesh) + 0);
	*vertCount = originalMesh.verts.size();

	subMeshCount = (reinterpret_cast<uint32_t*>(mesh) + 1);
	*subMeshCount = originalMesh.indicies.size();

	subMeshIndexCounts = (reinterpret_cast<uint32_t*>(mesh) + 2);


	size_t i = 0;
	for (std::vector<uint32_t>& subMesh : originalMesh.indicies) {
		*(subMeshIndexCounts + i) = subMesh.size();
		i++;
	}


	//verticies
	// this is the byte immediately after the proceidng array of all submesh lengths  
	glm::vec3* vertPosStart = reinterpret_cast<glm::vec3*>(reinterpret_cast<uint32_t*>(mesh) + 2 + i);
	memcpy(vertPosStart, originalMesh.verts.data(), originalMesh.vertsSize());

	glm::vec2* uvStart = reinterpret_cast<glm::vec2*>(vertPosStart + originalMesh.verts.size());
	memcpy(uvStart, originalMesh.uvs.data(), originalMesh.uvsSize());

	glm::vec3* normalStart = reinterpret_cast<glm::vec3*>(uvStart + originalMesh.uvs.size());
	memcpy(normalStart, originalMesh.normals.data(), originalMesh.normalsSize());

	glm::vec3* tangentStart = normalStart + originalMesh.normals.size();
	memcpy(tangentStart, originalMesh.tangents.data(), originalMesh.tangentsSize());

	glm::vec3* bitangentStart = tangentStart + originalMesh.tangents.size();
	memcpy(bitangentStart, originalMesh.bitangents.data(), originalMesh.bitangentsSize());

	//indices
	{
		uint32_t* indiciesStart = reinterpret_cast<uint32_t*>(bitangentStart + originalMesh.bitangents.size());
		size_t offset = 0;
		for (std::vector<uint32_t>& subMesh : originalMesh.indicies)
		{
			size_t size = sizeof(uint32_t) * subMesh.size();
			memcpy(indiciesStart + offset, subMesh.data(),size);
			offset += size;
		}
		assert((indiciesStart - mesh) + offset == meshLength);
	}
}

BinaryMeshSeirilizer::BinaryMeshSeirilizer(void* encodedMesh, size_t encodedMeshLength)
	: mesh(encodedMesh), meshLength(encodedMeshLength),
	vertCount((reinterpret_cast<uint32_t*>(encodedMesh))),
	subMeshCount((reinterpret_cast<uint32_t*>(encodedMesh) + 1)),
	subMeshIndexCounts((reinterpret_cast<uint32_t*>(encodedMesh) + 2))
{

}
