
#include "math/box.h"
#include "math/Math.h"
#include <iostream>
#include "glm/glm.hpp"
#include "osm requesting and parsing/OsmFetcher.h"

int main() {

	auto fetcher = new OsmFetcher();

	Box testChunk = { glm::dvec2(40.6056,-74.0713), glm::dvec3(0.0000001) };

	fetcher->fetchChunk(testChunk);

	return 0;
}