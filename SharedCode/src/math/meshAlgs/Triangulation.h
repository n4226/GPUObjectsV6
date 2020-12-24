#pragma once

#include "pch.h"


#include "rendering structures/BinaryMesh.h"
#include "../Box.h"

namespace meshAlgs
{

	struct TriangulatedMesh
	{
		std::vector<glm::dvec2> verts;
		std::vector<std::vector<glm::uint32_t>> indicies;
	};

	/// <summary>
	/// array of ppolygons that make up shape
	/// first polygon is base shape 
	/// all other polygons are holes 
	/// 
	/// the polygon returned is the pol.ygon made up of the first polygon points given
	/// </summary>
	std::pair<TriangulatedMesh*, bool> triangulate(std::vector<std::vector<glm::dvec2>>& polygon);

	
	Box bounds(std::vector<glm::dvec2>& points);


	void makeLibiglMesh(const BinaryMeshSeirilizer::Mesh& mesh, size_t subMesh, Eigen::MatrixXd& verts, Eigen::MatrixXi& indicies);
	void makeMeshFromLibigl(BinaryMeshSeirilizer::Mesh& mesh, size_t subMesh, const Eigen::MatrixXd& verts, const Eigen::MatrixXi& indicies);

};

