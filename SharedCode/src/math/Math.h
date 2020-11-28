#pragma once

#define _USE_MATH_DEFINES
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace Math {

	// constants

	constexpr auto dEarthRad = 6'378'137.0;
	constexpr auto fEarthRad = 6'378'137.f;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="lla">expected domain: [-90,90],[-180,180]</param>
	/// <param name="origin"></param>
	/// <param name="radius"></param>
	/// <returns></returns>
	glm::dvec3 LlatoGeo(glm::dvec3 lla, glm::dvec3 origin = glm::dvec3(),double radius = dEarthRad);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="lla">expected domain: [-90,90],[-180,180]</param>
	/// <param name="origin"></param>
	/// <param name="radius"></param>
	/// <returns></returns>
	glm::vec3 LlatoGeo(glm::vec3 lla, glm::vec3 origin = glm::dvec3(),float radius = fEarthRad);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="geo"></param>
	/// <param name="radius">if left 0 rad will be found from the length of geo</param>
	/// <param name="origin"></param>
	/// <returns></returns>
	glm::dvec3 GeotoLla(glm::dvec3 geo, glm::float64 radius = 0, glm::dvec3 origin = glm::dvec3());
	/// <summary>
	/// 
	/// </summary>
	/// <param name="geo"></param>
	/// <param name="radius">if left 0 rad will be found from the length of geo</param>
	/// <param name="origin"></param>
	/// <returns></returns>
	glm::vec3 GeotoLla(glm::vec3 geo, glm::float32 radius = 0, glm::vec3 origin = glm::vec3());

	// LLA Distance

	double llaDistance(glm::dvec2 from,glm::dvec2 to,double radius = dEarthRad);

	glm::quat fromToRotation(glm::vec3 startingDirection, glm::vec3 endingDirection);
}

