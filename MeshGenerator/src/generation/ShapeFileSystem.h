#pragma once

#include "math/Box.h"
#include <shapelib-master/shapefil.h>
#include <array>
#include <vector>

class ShapeFileSystem
{
public:

	ShapeFileSystem();
	~ShapeFileSystem();

	void getCoastlineInChunk(Box chunk);


private:

	int numberOfShapes;
	int fileShapeType;
	std::array<double, 4> padfMinBound;
	std::array<double, 4> padfMaxBound;

	std::vector<SHPObject*> shapes;


	SHPHandle shapeFile;
};

extern ShapeFileSystem* shapeFileSystem;