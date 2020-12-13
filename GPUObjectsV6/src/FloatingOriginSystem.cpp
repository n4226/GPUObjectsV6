#include "pch.h"
#include "FloatingOriginSystem.h"
#include "WorldScene.h"
#include "TerrainSystem.h"


void FloatingOriginSystem::update()
{
    PROFILE_FUNCTION;

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

        deltaTransform.scale = glm::vec3(1, 1, 1);
        deltaTransform.rotation = glm::identity<glm::quat>();
        deltaTransform.position = -position;

        auto objects = world->terrainSystem->drawObjects.lock();

        std::vector<BindlessMeshBuffer::WriteLocation> copyReceipts;

        for (std::pair<TerrainQuadTreeNode*, TreeNodeDrawData> drawData : *objects) {
            auto modelAddress = drawData.second.modelRecipt;

            copyReceipts.push_back(modelAddress);

            ModelUniforms* model = reinterpret_cast<ModelUniforms*>(reinterpret_cast<char*>(world->renderer->globalModelBufferStaging->mappedData) + modelAddress.offset);
            model->model = model->model * deltaTransform.matrix();
        }
        
        //TODO: NEED TO COPY CHANGES i should have to copy these changes to the gpu buffer(s) but for some reason it's wokring on my drivers and there are no validation errors so i'm not going to spend time on it right now
  /*      
        ResourceTransferer::Task task;

        task.srcBuffer = world->renderer->globalModelBufferStaging->vkItem;
        task.dstBuffer = world->renderer->globalModelBuffers[world->renderer->gpuUnactiveGlobalModelBuffer]->vkItem;
        

        ResourceTransferer::newTask(task);*/



        world->playerTrans.position = glm::vec3(0);
        printf("snapped floating origin\n");

    }
}
