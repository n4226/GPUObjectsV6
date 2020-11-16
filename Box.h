#pragma once


#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <ostream>

struct Box
{
	glm::dvec2 start, size;

	glm::dvec2 getEnd();
	glm::dvec2 getCenter();

	bool contains(glm::dvec2 point);
	//bool contains(Box other);


};

std::ostream& operator<<(std::ostream& strm, const Box& a);
