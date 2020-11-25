#include "pch.h"
#include "CameraSystem.h"
#include "WorldScene.h"

void CameraSystem::update()
{
	// the camera looks at -> +z
	
	world->playerTrans.position = Math::LlatoGeo(world->playerLLA, world->origin, world->terrainSystem->getRadius());

	world->playerTrans.rotation = glm::lookAt(world->playerTrans.position, glm::vec3(0), glm::vec3(0, 1, 0));

}