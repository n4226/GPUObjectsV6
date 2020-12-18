
#include "math/box.h"
#include "math/Math.h"
#include <iostream>
#include "glm/glm.hpp"
#include "osm requesting and parsing/OsmFetcher.h"

#include "generation/base/GenerationSystem.h"

#include "math/triangulation/Triangulation.h"

int main() {


	Triangulation::triangulate({});

	glm::dvec2 desired(40.610319941413, -74.039182662964);

	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(3,3)));

	GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(12,12)));
	
	//genSys.generate();


	/*s{
		GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(27, 27)));

		genSys.generate();
	}
	{
		GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(9, 9)));

		genSys.generate();
	}
	{
		GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 13, glm::ivec2(27, 27)));

		genSys.generate();
	}*/
	return 0;
}