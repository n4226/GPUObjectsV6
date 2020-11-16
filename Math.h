#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"

// constants

constexpr auto dEarthRad = 6378137.0;
constexpr auto fEarthRad = 6378137.f;

class Math
{
public:
	Math() = delete;
	// constants

	/// <summary>
	/// 
	/// </summary>
	/// <param name="lla">expected domain: [-90,90],[-180,180]</param>
	/// <param name="origin"></param>
	/// <param name="radius"></param>
	/// <returns></returns>
	static glm::dvec3 LlatoGeo(glm::dvec3 lla, glm::dvec3 origin = glm::dvec3(),double radius = dEarthRad);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="lla">expected domain: [-90,90],[-180,180]</param>
	/// <param name="origin"></param>
	/// <param name="radius"></param>
	/// <returns></returns>
	static glm::vec3 LlatoGeo(glm::vec3 lla, glm::vec3 origin = glm::dvec3(),float radius = fEarthRad);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="geo"></param>
	/// <param name="radius">if left 0 rad will be found from the length of geo</param>
	/// <param name="origin"></param>
	/// <returns></returns>
	static glm::dvec3 GeotoLla(glm::dvec3 geo, glm::float64 radius = 0, glm::dvec3 origin = glm::dvec3());
	/// <summary>
	/// 
	/// </summary>
	/// <param name="geo"></param>
	/// <param name="radius">if left 0 rad will be found from the length of geo</param>
	/// <param name="origin"></param>
	/// <returns></returns>
	static glm::vec3 GeotoLla(glm::vec3 geo, glm::float32 radius = 0, glm::vec3 origin = glm::vec3());

private:
};

