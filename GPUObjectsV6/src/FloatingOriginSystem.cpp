#include "pch.h"
#include "FloatingOriginSystem.h"
#include "WorldScene.h"


void FloatingOriginSystem::update()
{
    auto position = glm::dvec3(world->playerTrans.position);
    auto distance = glm::length(position);
    if (distance > FLOATING_ORIGIN_SNAP_DISTANCE) {
        // when getting acutal world positon oposit operation as below must be used
        world->origin += position;
        //            objectHolder.floatingUpdating
        //                .entityAndComponents
        //                .forEach { (entity,_, transform) in
        //                    transform.value.position -= position.vector32
        //                    objectHolder.forceUpdate(entity: entity)
        //            }
        // move objects
        world->playerTrans.position = glm::vec3(0);
        printf("snapped floating origin\n");
    }
}
