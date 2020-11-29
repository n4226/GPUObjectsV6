#include "pch.h"
#include "CameraSystem.h"
#include "WorldScene.h"

void CameraSystem::update()
{
	// move around the world fast
	//world->playerLLA = glm::dvec3(45 + sin(world->timef * 0.1f), 0, 10100 + -cos(world->timef * 0.5f) * 10'000);

	//world->playerLLA = glm::dvec3(sin(world->timef * 0.8f) * 6, sin(world->timef * 1.f) * 9, 1'000);
	//world->playerLLA = glm::dvec3(sin(world->timef * 0.8f) * 60, 0, 1'000);
	//world->playerLLA = glm::dvec3(0, sin(world->timef * 1.f) * 90, 1'000);

	// move up and down
	//world->playerLLA.z = -cos(world->timef) * 10000 + 10100;
	// the camera looks at -> +z
	
	world->playerTrans.position = Math::LlatoGeo(world->playerLLA, world->origin, world->terrainSystem->getRadius());

	//world->playerTrans.rotation = glm::orientation(glm::vec3(0,0,1), glm::normalize(world->playerTrans.position));

	//world->playerTrans.rotation = glm::lookAt(glm::vec3(0), -glm::vec3(world->origin), glm::vec3(0, 0, -1));

	// sort of works - posible additnio to up = glm::normalize(world->playerTrans.position)
	//world->playerTrans.rotation = glm::lookAt(glm::normalize(world->playerTrans.position), glm::vec3(0), glm::vec3(0, 0, -1));

	// rotation ------



	// sin form [0,1] - * (sin(world->timef) * 0.5f + 1)

	auto N = glm::normalize(world->playerTrans.position + glm::vec3(world->origin));

	auto QuatAroundX = glm::angleAxis(glm::radians(static_cast<float>(world->playerLLA.x)), glm::vec3(1.0, 0.0, 0.0));
	// when at 0 lat and 0 lon to look directly at sphere the quataroundy must have an angle of .pi/2 rad
	auto QuatAroundY = glm::angleAxis(-glm::pi<float>() / 2 - glm::radians(static_cast<float>(world->playerLLA.y)), glm::vec3(0.0, 1.0, 0.0));
	auto QuatAroundZ = glm::angleAxis(0.f, glm::vec3(0.0, 0.0, 1.0));
	auto finalOrientation = QuatAroundZ * QuatAroundY * QuatAroundX;

	world->playerTrans.rotation = finalOrientation * glm::angleAxis(glm::radians(90.f), glm::vec3(-1, 0, 0));

		//glm::
		//static_cast<glm::quat>(glm::lookAt(glm::vec3(0), glm::cross(N,glm::vec3(0,0,1)),-N));


		//glm::angleAxis(glm::radians(90.f), glm::vec3(0,-1, 0)) *
		//Math::fromToRotation(glm::vec3(0, -1, 0), glm::normalize(world->playerTrans.position + glm::vec3(world->origin)));
		//* glm::angleAxis(glm::radians(90.f), glm::vec3(-1, 0, 0));

		//glm::angleAxis(static_cast<float>(glm::radians(world->playerLLA.x)) ,glm::vec3(1,0,0));

		//glm::angleAxis(static_cast<float>(glm::radians(world->playerLLA.y)) ,glm::vec3(0,1,0));


	//world->playerTrans.rotation = Math::fromToRotation(glm::vec3(0, 0, -1), -glm::normalize(world->playerTrans.position + glm::vec3(world->origin)))
		//temporary
		//* glm::angleAxis(glm::radians(static_cast<float>(world->playerLLA.y)), glm::vec3(0, -1, 0))
		//* glm::angleAxis(glm::radians(90.f * world->timef), glm::vec3(-1, 0, 0));
		//;
}
