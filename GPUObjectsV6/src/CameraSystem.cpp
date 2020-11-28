#include "pch.h"
#include "CameraSystem.h"
#include "WorldScene.h"

void CameraSystem::update()
{
	// move around the world fast
	//world->playerLLA = glm::dvec3(45 + sin(world->timef * 0.1f), 0, 10100 + -cos(world->timef * 0.5f) * 10'000);

	//world->playerLLA = glm::dvec3(0,0 + sin(world->timef * 0.1f) * 20, 10'000);

	// move up and down
	//world->playerLLA.z = -cos(world->timef) * 10000 + 10100;
	// the camera looks at -> +z
	
	world->playerTrans.position = Math::LlatoGeo(world->playerLLA, world->origin, world->terrainSystem->getRadius());

	//world->playerTrans.rotation = glm::orientation(glm::vec3(0,0,1), glm::normalize(world->playerTrans.position));

	//world->playerTrans.rotation = glm::lookAt(glm::vec3(0), -glm::vec3(world->origin), glm::vec3(0, 0, -1));

	// sort of works - posible additnio to up = glm::normalize(world->playerTrans.position)
	//world->playerTrans.rotation = glm::lookAt(glm::normalize(world->playerTrans.position), glm::vec3(0), glm::vec3(0, 0, -1));


	world->playerTrans.rotation = Math::fromToRotation(glm::vec3(0, 0, -1), -glm::normalize(world->playerTrans.position + glm::vec3(world->origin)))
		//temporary
		* glm::angleAxis(glm::radians(static_cast<float>(world->playerLLA.y)), glm::vec3(0, -1, 0))
		* glm::angleAxis(glm::radians(90.f * world->timef), glm::vec3(-1, 0, 0));
		//* glm::angleAxis(glm::radians(90.f), glm::vec3(-1, 0, 0));
}
