#pragma once

#include "rendering structures/BinaryMesh.h"
#include "../osm requesting and parsing/OsmFetcher.h"

class icreator
{
public:
	
	virtual void createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame);

private:

};
