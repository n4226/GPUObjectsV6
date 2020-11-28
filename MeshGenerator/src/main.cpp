
#include "math/box.h"
#include "math/Math.h"
#include <iostream>
#include "glm/glm.hpp"
#include "osm requesting and parsing/OsmFetcher.h"

#include "generation/base/GenerationSystem.h"

int main() {

	glm::dvec2 desired(40.610319941413, -74.039182662964);

	GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(3,3)));

	genSys.generate();

	return 0;
}