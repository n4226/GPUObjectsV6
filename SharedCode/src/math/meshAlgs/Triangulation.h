#pragma once

#include "pch.h"


#include "rendering structures/BinaryMesh.h"

namespace meshAlgs
{

	/// <summary>
	/// array of ppolygons that make up shape
	/// first polygon is base shape 
	/// all other polygons are holes 
	/// </summary>
	BinaryMeshSeirilizer::Mesh* triangulate(std::vector<std::vector<glm::vec2>> polygon);


	void makeLibiglMesh(const BinaryMeshSeirilizer::Mesh& mesh, size_t subMesh, Eigen::MatrixXd& verts, Eigen::MatrixXi& indicies);
	void makeMeshFromLibigl(BinaryMeshSeirilizer::Mesh& mesh, size_t subMesh, const Eigen::MatrixXd& verts, const Eigen::MatrixXi& indicies);

};

