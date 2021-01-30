#pragma once
#include "../creator.h"

class RoadCreator : public icreator
{
public:
	void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame, int lod) override;

private:

	void addRoad(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, osm::element& road, const Box& frame, int lod);


};