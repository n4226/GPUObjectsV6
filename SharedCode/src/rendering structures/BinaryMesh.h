#pragma once

#include<glm/glm.hpp>
#include <vector>



template<uint32_t vCount, uint32_t sCount, uint32_t ... indexCount>
struct __ExampleBinaryMesh {
	//header
	glm::uint32_t vertCount = vCount;
	glm::uint32_t subMeshCount = sCount;
	glm::uint32_t subMeshCounts[sCount];


	//body
	glm::vec3 verts[vCount];
	glm::vec2 uvs[vCount];
	glm::vec3 normals[vCount];
	glm::vec3 tangents[vCount];
	glm::vec3 bitangents[vCount];

	// THIS IS NOT expresable in c++ but this is an array of varing length arrays. one for each submesh
	glm::uint32_t subMeshes[sCount];

};


class BinaryMeshSeirilizer
{
public:
	/// <summary>
	/// a barbones mesh sctruct for mesh gen program
	/// </summary>
	struct Mesh
	{
		std::vector<glm::vec3> verts;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		std::vector<std::vector<glm::uint32>> indicies;


		// offset and size functions

		size_t         vertsSize();
		size_t           uvsSize();
		size_t       normalsSize();
		size_t      tangentsSize();
		size_t    bitangentsSize();
		size_t      indiciesSize();

		size_t       vertsOffset();
		size_t         uvsOffset();
		size_t     normalsOffset();
		size_t    tangentsOffset();
		size_t  bitangentsOffset();
		size_t    indiciesOffset();

		size_t    fullSize();
	};


	BinaryMeshSeirilizer(Mesh& originalMesh);
	BinaryMeshSeirilizer(void* encodedMesh, size_t encodedMeshLength);
	BinaryMeshSeirilizer(const char* filePath);
	~BinaryMeshSeirilizer();

	glm::uint32_t* vertCount;
	glm::uint32_t* subMeshCount;
	// this is an array
	glm::uint32_t* subMeshIndexCounts;
	size_t headerLength;

	/// <summary>
	/// the actuall full binary mesh
	/// </summary>
	void* mesh;
	size_t meshLength;

	size_t         vertsSize();
	size_t           uvsSize();
	size_t       normalsSize();
	size_t      tangentsSize();
	size_t    bitangentsSize();
	size_t      indiciesSize(size_t subMesh);

	void*         vertsPtr();
	void*           uvsPtr();
	void*       normalsPtr();
	void*      tangentsPtr();
	void*    bitangentsPtr();

	void*      indiciesPtr(size_t subMesh);

};

