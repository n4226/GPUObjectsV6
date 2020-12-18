#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Triangulation
{

	/// <summary>
	/// array of ppolygons that make up shape
	/// first polygon is base shape 
	/// all other polygons are holes 
	/// </summary>
	std::vector<glm::uint32> triangulate(std::vector<std::vector<glm::vec2>> polygon);

};

