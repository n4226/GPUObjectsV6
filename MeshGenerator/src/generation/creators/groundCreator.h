#pragma once

#include "../creator.h"

class groundCreator: public icreator
{
public:
	void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame) override;

private:

	void createChunkMesh(BinaryMeshSeirilizer::Mesh* mesh, const Box& frame);
};

