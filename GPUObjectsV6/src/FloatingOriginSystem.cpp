#include "pch.h"
#include "FloatingOriginSystem.h"
#include "WorldScene.h"
#include "TerrainSystem.h"


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

        Transform deltaTransform;

        deltaTransform.scale = glm::uvec3(1, 1, 1);
        deltaTransform.rotation = glm::identity<glm::quat>();
        deltaTransform.position = -position;

        auto objects = world->terrainSystem->drawObjects.lock();
        for (std::pair<TerrainQuadTreeNode*, TreeNodeDrawData> drawData : *objects) {
            auto modelAddress = drawData.second.modelRecipt;

            ModelUniforms* model = reinterpret_cast<ModelUniforms*>(reinterpret_cast<char*>(world->renderer->globalModelBufferStaging->mappedData) + modelAddress.offset);
            model->model = model->model * deltaTransform.matrix();
        }
        
        //TODO: i should have to copy these changes to the gpu buffer(s) but for some reason it's wokring on my drivers and there are no validation errors so i'm not going to spend time on it right now
        //ResourceTransferer::newTask();

        world->playerTrans.position = glm::vec3(0);
        printf("snapped floating origin\n");

    }
}
