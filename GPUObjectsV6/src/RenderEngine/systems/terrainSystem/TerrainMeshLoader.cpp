#include "pch.h"
#include "TerrainMeshLoader.h"
#include "TerrainSystem.h"
#include "../RenderSystems/Renderer.h"
#include <filesystem>
#include "../../../Application/FileManager.h"




TreeNodeDrawResaourceToCoppy TerrainMeshLoader::loadMeshPreDrawChunk(TerrainQuadTreeNode* node, bool inJob)
{
	// get  and encode mesh
	//TODO store this in a better place
	const auto Terrain_Chunk_Mesh_Dir = FileManager::getTerrainChunkDir();//R"(terrain/chunkMeshes/)";

	TreeNodeDrawResaourceToCoppy meshStore;

	auto file = Terrain_Chunk_Mesh_Dir + node->frame.toString() + ".bmesh";
	if (std::filesystem::exists(file)) {
		PROFILE_SCOPE("loading mesh from file")

			auto mesh = new BinaryMeshSeirilizer(file.c_str());

		meshStore.binMesh = mesh;
	}
	else
	{
		PROFILE_SCOPE("creating empty mesh")

			auto mesh = createChunkMesh(*node);

		meshStore.mesh = mesh;
	}

	if (inJob) { // sync point
		auto meshStores = terrainSystem->loadedMeshesToDraw.lock();
		(*meshStores)[node] = meshStore;
	}
	return meshStore;
}

void TerrainMeshLoader::drawChunk(TerrainQuadTreeNode* node, TreeNodeDrawResaourceToCoppy preLoadedMesh, bool inJob)
{
	PROFILE_FUNCTION

		//TODO - probablty more efficiant if all buffer writes were done after the draw objects were cxreatted all at once (in the write draw commands function) to allow single mapping and unmaopping and so on
		// this would mean that this funciton would have to crateda nother object with temporary object eg mesh and model trans to be writien at the end of the system update
		if (node->hasdraw) return;

	// get thread local buffers for writting to

	BindlessMeshBuffer* stagingMeshBuff;
	Buffer* stagingModelBuff;

	if (!inJob) {
		stagingMeshBuff = renderer->globalMeshStagingBuffer;
		stagingModelBuff = renderer->globalModelBufferStaging;
	}
	else {
		auto id = std::this_thread::get_id();

		bool needAllocate = false;

		{
			auto meshThreadMap = renderer->ThreadLocalGlobalMeshandModelStagingBufferThreadIndicies.lock_shared();
			if (meshThreadMap->count(id) > 0) {
				stagingMeshBuff = renderer->threadLocalGlobalMeshStagingBuffers[meshThreadMap->at(id)];
				stagingModelBuff = renderer->threadLocalGlobalModelStagingBuffers[meshThreadMap->at(id)];
			}
			else {
				needAllocate = true;
			}
		}

		if (needAllocate) {

			size_t index;

			{// sync point
				auto meshThreadFree = renderer->freeThreadLocalGlobalMeshandModelStagingBufferIndicies.lock();

				assert(meshThreadFree->size() > 0);
				index = (*meshThreadFree)[meshThreadFree->size() - 1];
				meshThreadFree->pop_back();
			}


			{// sync point
				auto meshThreadMap = renderer->ThreadLocalGlobalMeshandModelStagingBufferThreadIndicies.lock();
				(*meshThreadMap).insert(std::pair<std::thread::id, size_t>(id, index));

				stagingMeshBuff = renderer->threadLocalGlobalMeshStagingBuffers[meshThreadMap->at(id)];
				stagingModelBuff = renderer->threadLocalGlobalModelStagingBuffers[meshThreadMap->at(id)];
			}
		}
		

		stagingMeshBuff = renderer->globalMeshStagingBuffer;
		stagingModelBuff = renderer->globalModelBufferStaging;
	}

	// functiuon global state
	BindlessMeshBuffer::WriteTransactionReceipt meshReceipt;

	VkDeviceAddress vertIndex;
	VkDeviceAddress indIndex;
	size_t vertCount;
	size_t indCount;

	//TreeNodeDrawResaourceToCoppy preLoadedMesh;

	if (inJob || (preLoadedMesh.binMesh == nullptr && preLoadedMesh.mesh == nullptr)) { // sync point
		auto meshStores = terrainSystem->loadedMeshesToDraw.lock();
		preLoadedMesh = (*meshStores)[node];
	}

	if (preLoadedMesh.binMesh != nullptr) {
		PROFILE_SCOPE("loading with pre loaded mesh from file")
		BinaryMeshSeirilizer& mesh = *preLoadedMesh.binMesh;

		vertCount = *mesh.vertCount;
		indCount = mesh.subMeshIndexCounts[0];

		// these indicies are in vert count space - meaning 1 = 1 vert not 1 byte
		vertIndex = renderer->gloablVertAllocator->alloc(vertCount);
		indIndex = renderer->gloablIndAllocator->alloc(indCount);

		stagingMeshBuff->writeMeshToBuffer(vertIndex, indIndex, &mesh, !inJob);


		//renderer->globalMeshStaging->vertBuffer->gpuCopyToOther(renderer->globalMesh->vertBuffer)
		meshReceipt = stagingMeshBuff->genrateWriteReceipt(vertIndex, indIndex, &mesh);
	}
	else
	{
		PROFILE_SCOPE("loading with pre creating empty mesh")
		auto mesh = preLoadedMesh.mesh;

		vertCount = mesh->verts.size();
		indCount = mesh->indicies.size();
		
		// these indicies are in vert count space - meaning 1 = 1 vert not 1 byte
		vertIndex = renderer->gloablVertAllocator->alloc(mesh->verts.size());
		indIndex = renderer->gloablIndAllocator->alloc(mesh->indicies.size());

		stagingMeshBuff->writeMeshToBuffer(vertIndex, indIndex, mesh, !inJob);


		//renderer->globalMeshStaging->vertBuffer->gpuCopyToOther(renderer->globalMesh->vertBuffer)
		meshReceipt = stagingMeshBuff->genrateWriteReceipt(vertIndex, indIndex, mesh);
	}

	// model 

	Transform transform;
	transform.position = node->center_geo - *terrainSystem->origin;
	transform.scale = glm::vec3{ 1,1,1 };
	transform.rotation = glm::identity<glm::quat>();

	ModelUniforms mtrans;
	mtrans.model = transform.matrix();



	auto modelIndex = renderer->globalModelBufferAllocator->alloc();
	auto modelAllocSize = renderer->globalModelBufferAllocator->allocSize;

	stagingModelBuff->tempMapAndWrite(&mtrans, modelIndex, modelAllocSize, !inJob);



	TreeNodeDrawData drawData;
	drawData.indIndex = indIndex;
	drawData.vertIndex = vertIndex;
	drawData.vertcount = vertCount;
	drawData.indexCount = indCount;
	drawData.meshRecipt = meshReceipt;
	drawData.modelRecipt = { modelIndex,modelAllocSize };

	drawData.drawData.modelIndex = static_cast<glm::uint32>(modelIndex / modelAllocSize);


	//TODO fix this to be precomputed 

	drawData.aabbMin = transform.position + glm::vec3(-1000);
	drawData.aabbMax = transform.position + glm::vec3(1000);

	/*for (size_t i = 0; i < length; i++)
	{

	}*/


	node->hasdraw = true;

	auto drawQueue = terrainSystem->pendingDrawObjects.lock();
	(*drawQueue)[node] = drawData;

}

void TerrainMeshLoader::removeDrawChunk(TerrainQuadTreeNode* node, bool inJob)
{
	PROFILE_FUNCTION

		auto drawObjects = terrainSystem->drawObjects.lock();
	//printf("%d before \n", drawObjects->size());
	node->hasdraw = false;
	auto draw = (*drawObjects)[node];
	drawObjects->erase(node);
	//printf("%d after \n", drawObjects->size());

	//TODO: deallocate buffers here 

	renderer->gloablVertAllocator->free(draw.vertIndex, draw.vertcount);
	renderer->gloablIndAllocator->free(draw.indIndex, draw.indexCount);

}













Mesh* TerrainMeshLoader::createChunkMesh(const TerrainQuadTreeNode& chunk)
{
    PROFILE_FUNCTION

    Mesh* mesh = new Mesh();

    size_t resolution = 10;
    auto d_resolution = static_cast<double>(resolution);
    const Box& frame = chunk.frame;

    glm::uint32 vert = 0;
    const glm::uint32 startVertOfset = 0;

    for (size_t x = 0; x <= resolution; x++)
    {
        for (size_t y = 0; y <= resolution; y++)
        {
            // vertex creation

            auto chunkStrideLat = (frame.size.x / d_resolution) * static_cast<double>(x);
            auto chunkStrideLon = (frame.size.y / d_resolution) * static_cast<double>(y);

            auto lat = frame.start.x + chunkStrideLat;
            auto lon = frame.start.y + chunkStrideLon;
            glm::dvec3 lla(lat, lon, 0);

            auto geo_unCentered = Math::LlatoGeo(lla, {}, chunk.tree->radius);

            mesh->verts.emplace_back(geo_unCentered - chunk.center_geo);//Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius));
#if DEBUG
            mesh->normals.emplace_back(static_cast<glm::vec3>(glm::normalize(geo_unCentered)) * (static_cast<float>(chunk.lodLevel + 1) / 13.f));
#else
            mesh->normals.emplace_back(static_cast<glm::vec3>(glm::normalize(geo_unCentered)));
#endif
            // chunk uvs
            auto uvx = chunkStrideLat / frame.size.x;
            auto uvy = chunkStrideLon / frame.size.y;
            // world uvs
//                    let uvx = (y.double - frame.origin.y) / resolution.double / (360 / frame.size.y) + ((frame.origin.y + 180) / 360)
//                    let uvy = (x.double - frame.origin.x) / resolution.double / (180 / frame.size.x) + ((frame.origin.x + 90) / 180)
            // world uvs 2
//                    let uvy = ((lla.x + 90) / 180)
//                    let uvx = ((lla.y + 180) / 360)
//                    if uvx > 1 {
//                        uvx -= 1
//                    }

            mesh->uvs.emplace_back(static_cast<float>(uvx), static_cast<float>(uvy));

        }
    }

    for (size_t x = 0; x < resolution; x++)
    {
        for (size_t y = 0; y < resolution; y++)
        {
            // indicies creation

            auto xSize = resolution;

            mesh->indicies.emplace_back(vert + startVertOfset            );
            mesh->indicies.emplace_back(vert + startVertOfset + xSize + 1);
            mesh->indicies.emplace_back(vert + startVertOfset + 1        );
            mesh->indicies.emplace_back(vert + startVertOfset + 1        );
            mesh->indicies.emplace_back(vert + startVertOfset + xSize + 1);
            mesh->indicies.emplace_back(vert + startVertOfset + xSize + 2);

            vert++;
        }
        vert++;
    }

    mesh->tangents.resize(mesh->verts.size());
    mesh->bitangents.resize(mesh->verts.size());

    return mesh;
}