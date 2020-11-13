#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//#include <glm/gtc/quaternion.hpp>

struct MyStruct
{
public:
	
	glm::vec3 position;
	glm::vec3 scale;
	glm::qua<glm::float32> rotation;

	static glm::mat4 matrix();

};

