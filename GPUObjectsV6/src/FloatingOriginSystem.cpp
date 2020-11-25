#include "pch.h"
#include "FloatingOriginSystem.h"
#include "WorldScene.h"
#include "TerrainSystem.h"


void FloatingOriginSystem::update()
{
    //return;
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

        for (std::pair<TerrainQuadTreeNode*, TreeNodeDrawData> drawData : world->terrainSystem->drawObjects) {
            auto modelAddress = drawData.second.modelRecipt;

            ModelUniforms* model = reinterpret_cast<ModelUniforms*>(reinterpret_cast<char*>(world->renderer->globalModelBufferStaging->mappedData) + modelAddress.offset);
            model->model = model->model * deltaTransform.matrix();
        }

        ResourceTransferer::newTask();

        world->playerTrans.position = glm::vec3(0);
        printf("snapped floating origin\n");

    }
}
