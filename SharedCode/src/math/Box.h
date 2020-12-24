#pragma once


#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <ostream>
#include <vector>


struct Box
{
	Box();
	Box(glm::dvec2 start, glm::dvec2 size);

	glm::dvec2 start, size = glm::dvec3(0);

	glm::dvec2 getEnd() const;
	glm::dvec2 getCenter() const;

	bool contains(glm::dvec2 point) const;
	bool containsAny(std::vector<glm::dvec2>& points) const;
	bool overlaps(Box other) const;
	//bool contains(Box other);

	std::string toString() const;
};

std::ostream& operator<<(std::ostream& strm, const Box& a);
