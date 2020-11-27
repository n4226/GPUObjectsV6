#pragma once

#include "math/Box.h"
#include "math/Math.h"
#include "osm.h"
#include <string>



class OsmFetcher
{
public:

	OsmFetcher();


	osm::osm fetchChunk(Box frame);

private:
	std::string getQuery(Box frame);
	std::string getTestQuery();
};

