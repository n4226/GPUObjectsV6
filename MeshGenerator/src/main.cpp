
#include "math/box.h"
#include "math/Math.h"
#include <iostream>
#include "glm/glm.hpp"
#include "osm requesting and parsing/OsmFetcher.h"
#include "generation/ShapeFileSystem.h"

#include "generation/base/GenerationSystem.h"

#include "math/meshAlgs/Triangulation.h"
#include "math/meshAlgs/MeshRendering.h"

#include "marl/scheduler.h"
#include "marl/task.h"
#include "marl/defer.h"

int main() {

	auto confic = marl::Scheduler::Config();

	confic.setWorkerThreadCount(std::thread::hardware_concurrency());

	auto scheduler = new marl::Scheduler(confic);

	scheduler->bind();
	defer(scheduler->unbind());

	shapeFileSystem = new ShapeFileSystem();


	glm::dvec2 desired(40.610319941413, -74.039182662964);

	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(3, 3)));
	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(3, 3)));

	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(12,12)));
	//GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired,12, glm::ivec2(27,27)));


	//genSys.generate(0,false);

	// first parm is chunk index and second is lod level
	//genSys.debugChunk(0,1);

	//auto m = meshAlgs::triangulate({ { {-2,-2}, {2,-2}, {2,2}, {-2, 2} }, { {-1,-1}, {-1,1}, {1,1}, {1,-1}} });
		//({ { {-2,-2}, {2,-2}, {2,2}, {-2, 2 } } });

	//meshAlgs::displayMesh(*m);

	{
		GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 12, glm::ivec2(27, 27)));

		genSys.generate(0,false);
	}
	{
		GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 11, glm::ivec2(9, 9)));

		genSys.generate(1,false);
	}
	/*{
		GenerationSystem genSys(GenerationSystem::genreateChunksAround(desired, 13, glm::ivec2(27, 27)));

		genSys.generate();
	}*/
	delete shapeFileSystem;
	return 0;
}