#include "ShapeFileSystem.h"

#include "constants.h"
#include <iostream>

#define Sunrise

#define SR_PROFILNG
#define SRE_PROFILNG

#define SR_GFX
#define SRE_GFX

#define SR_API

SR_API



namespace Sunrise {
	namespace Graphics {

	}
}

ShapeFileSystem::ShapeFileSystem()
{
	auto path = std::string(OSM_COASTLINE_DIR) + "land_polygons.shp";

	shapeFile = SHPOpen(path.c_str(),"rb");



	SHPGetInfo(shapeFile, &numberOfShapes, &fileShapeType, padfMinBound.data(), padfMaxBound.data());

		shapes.resize(numberOfShapes);

	for (size_t i = 0; i < numberOfShapes; i++)
	{
		shapes[i] = SHPReadObject(shapeFile, i);
	}


	//std::cout << "finished loading shape file" << std::endl;
}

ShapeFileSystem::~ShapeFileSystem()
{
	SHPClose(shapeFile);
}

void ShapeFileSystem::getCoastlineInChunk(Box chunk)
{

} 

ShapeFileSystem* shapeFileSystem = nullptr;