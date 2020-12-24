#pragma once

#include "../creator.h"

class buildingCreator: public icreator
{
public:
	void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame, int lod) override;

private:

	std::map<Box*,osm::element*> buldingAABBS;

	void addBuilding(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, osm::element& building, const Box& frame, int lod);

};

