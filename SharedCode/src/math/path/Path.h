#pragma once

#include "glm/glm.hpp"
#include "../Math.h"

#include <vector>
#include <array>

/// <summary>
/// ported with minor changes from: https://www.youtube.com/watch?v=nNmFLWup4_k&list=PLFt_AvWsXl0d8aDaovNztYf6iTChHzrHP&index=3 - video series
/// </summary>
/// <typeparam name="p">must be a glm vector type</typeparam>
template<typename p>
struct Path
{
	//Path();
	
	/// <summary>
	/// creates a path and generates control points form a vector of only anchor points
	/// </summary>
	/// <param name="anchorPoints"></param>
	/// <param name="closed">if closed will add segment from last anchor pos to first anchor pos</param>
	Path(std::vector<p> anchorPoints,bool closed);
	~Path();


	void addSegment(p anchorPoint);

	void autoSetAllContorlPoints();
	void autoSetAllAffectedControlPoints(size_t updatedAnchorIndex);
	void autoSetAnchorControlPoints(size_t anchorIndex);
	void autoSetStartAndEndContorls();

	std::array<p, 4> getPointsInSegment(size_t i);

	size_t getNumSegments();
	size_t getNumPoints();

	void setClosed(bool closed);
	bool getClosed();

	double directLength() {
		double length = 0;

		for (size_t i = 0; i < getNumSegments(); i++)
		{
			auto points = getPointsInSegment(i);
			length += glm::distance(points[0], points[2]);
		}
		return length;
	}


	double lladirectLength() {
		double length = 0;

		for (size_t i = 0; i < getNumSegments(); i++)
		{
			auto points = getPointsInSegment(i);
			length += Math::llaDistance(points[0], points[2]);
		}
		return length;
	}

private:

	size_t loopIndex(size_t i);

	bool closed;
	std::vector<p> points;
};


template<typename p>
Path<p>::Path(std::vector<p> anchorPoints, bool closed)
{
	closed = false;
	if (anchorPoints.size() < 2) return;

	points.emplace_back(anchorPoints[0]);

	// blank control points
	points.emplace_back(anchorPoints[0]);
	points.emplace_back(anchorPoints[1]);


	points.emplace_back(anchorPoints[1]);

	autoSetStartAndEndContorls();

	for (size_t i = 2; i < anchorPoints.size(); i++)
	{
		addSegment(anchorPoints[i]);
	}


	if (closed) {
		//setClosed(true);
	}

	autoSetAllContorlPoints();
}

template<typename p>
inline Path<p>::~Path()
{

}

template<typename p>
void Path<p>::addSegment(p anchorPoint)
{
	points.emplace_back(points[points.size() - 1] * 2.0 - points[points.size() - 2]);
	points.emplace_back((points[points.size() - 1] + anchorPoint) / 2.0);
	points.emplace_back(anchorPoint);
}

template<typename p>
void Path<p>::autoSetAllContorlPoints()
{
	for (size_t i = 0; i < points.size(); i += 3)
	{
		autoSetAnchorControlPoints(i);
	}
	autoSetStartAndEndContorls();
}

template<typename p>
void Path<p>::autoSetAllAffectedControlPoints(size_t updatedAnchorIndex)
{
	for (size_t i = updatedAnchorIndex - 3; i < updatedAnchorIndex + 3; i += 3)
	{
		if (closed || (i >= 0 && i < points.size()))
			autoSetAnchorControlPoints(i);
	}
	autoSetStartAndEndContorls();
}

template<typename p>
void Path<p>::autoSetAnchorControlPoints(size_t anchorIndex)
{
	auto anchorPos = points[anchorIndex];
	p dir(0);
	std::array<p, 2> neighborDistances;

	if (closed || anchorIndex - 3 >= 0) {
		auto offset = points[loopIndex(anchorIndex - 3)] - anchorPos;
		dir += glm::normalize(offset);
		neighborDistances[0] = p(glm::length(offset));
	}
	if (closed || anchorIndex + 3 >= 0 ) {// points.size()) {
		auto offset = points[loopIndex(anchorIndex + 3)] - anchorPos;
		dir -= glm::normalize(offset);
		neighborDistances[1] = p(-glm::length(offset));
	}

	dir = glm::normalize(dir);

	for (size_t i = 0; i < 2; i++)
	{
		auto controlIndex = anchorIndex + i * 2 - 1;
		if (closed || (controlIndex >= 0 && controlIndex < points.size())) {
			points[loopIndex(controlIndex)] = anchorPos + dir * neighborDistances[i].x * 0.5;
		}
	}
}

template<typename p>
void Path<p>::autoSetStartAndEndContorls()
{
	if (!closed) {
		points[1] = (points[0] + points[2]) * 0.5;
		points[points.size() - 2] = (points[points.size() - 1] + points[points.size() - 3]) * 0.5;
	}
}

template<typename p>
std::array<p, 4> Path<p>::getPointsInSegment(size_t i)
{
	return {
		points[(i * 3)],
		points[(i * 3 + 1)],
		points[(i * 3 + 2)],
		points[loopIndex(i * 3 + 3)]
	};
}

template<typename p>
size_t Path<p>::getNumSegments()
{
	return points.size() / 3;
}

template<typename p>
size_t Path<p>::getNumPoints()
{
	return points.size();
}

template<typename p>
void Path<p>::setClosed(bool closed)
{
	this->closed = closed;

	if (closed) {
		points.emplace_back(points[points.size() - 1] * 2.0 - points[points.size() - 2]);
		points.emplace_back(points[0] * 2.0 - points[1]);

	}
	else {
		points.pop_back();
		points.pop_back();
	}
}

template<typename p>
bool Path<p>::getClosed()
{
	return closed;
}

template<typename p>
size_t Path<p>::loopIndex(size_t i)
{
	return (i + points.size()) % points.size();
}
