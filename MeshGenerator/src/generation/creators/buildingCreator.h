#pragma once

#include "../creator.h"

class buildingCreator: public icreator
{
public:
	void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame) override;

private:

	void addBuilding(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, osm::element& building, const Box& frame);

};

