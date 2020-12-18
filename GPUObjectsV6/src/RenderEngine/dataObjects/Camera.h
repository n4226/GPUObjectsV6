#pragma once

#include "Transform.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

struct Camera
{
	/// <summary>
	/// degrees
	/// </summary>
	float fov;

	float zNear, zFar;


	Transform transform;

	glm::mat4 projection(float width, float height);
	glm::mat4 view();
	glm::mat4 viewProjection(float width, float height);

};

