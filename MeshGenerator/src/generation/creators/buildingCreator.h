#pragma once

#include "../creator.h"

class buildingCreator: public icreator
{
public:
	void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame) override;


};

