#pragma once

#include "glm/glm.hpp"
#include "math/Box.h"
#include <vector>
#include "../creator.h"
#include "../../osm requesting and parsing/OsmFetcher.h"

class GenerationSystem
{
public:
	GenerationSystem(std::vector<Box>&& chunks);
	

	void generate(int lod, bool onlyUseOSMCash = false);

	void debugChunk(size_t index, int lod);

	static std::vector<Box> genreateChunksAround(glm::dvec2 desired,int divided, glm::ivec2 formation);
private:
	static Box actualChunk(glm::dvec2 desired,int divided = 12);

	OsmFetcher osmFetcher;

	std::vector<Box> chunks;
	std::vector<icreator*> creators;
};

