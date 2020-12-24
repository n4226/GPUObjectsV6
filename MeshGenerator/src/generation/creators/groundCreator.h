#pragma once

#include "../creator.h"

class groundCreator: public icreator
{
public:
	void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame, int lod) override;


private:

	void createSubdividedQuadChunkMesh(BinaryMeshSeirilizer::Mesh & mesh, const Box& frame);

	void drawMultPolygonInChunk(std::vector<std::vector<glm::dvec2>>& polygon, BinaryMeshSeirilizer::Mesh& mesh, const Box& frame);

	std::vector<std::vector<glm::dvec2>>* createLandPolygonChunkMesh(BinaryMeshSeirilizer::Mesh& mesh, const Box& frame);
};

