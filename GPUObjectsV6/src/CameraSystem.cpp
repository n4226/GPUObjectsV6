#include "pch.h"
#include "CameraSystem.h"
#include "WorldScene.h"

void CameraSystem::update()
{
	// the camera looks at -> +z
	glm::vec3 axis = { 0,1,0 };
	auto rot = glm::angleAxis(glm::radians(60 * sin((float)world->time)), axis);

	world->playerTrans.position =
		//glm::vec3(0, 0.3, -1.8) * (cos(world->timef * 0.5f) * 10100.f + 1.f);
	//glm::vec3(rot * glm::vec4(0,0,-2,1));
		glm::vec3( 0, 0, -15010 - cos(world->timef * 1.f) * 5000 ) - glm::vec3(world->origin);

	auto newPos = world->playerTrans.position;

	/*newPos.x = newPos.z;
	newPos.z = playerTrans.position.x;*/

	//playerTrans.rotation = glm::angleAxis(glm::radians(45.f),glm::vec3(0,0,1)) * glm::quat(glm::lookAt(-newPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
	//Math::LlatoGeo(playerLLA, glm::dvec3(0, 0, 10),1);

}