#include "GenerationSystem.h"

#include "rendering structures/BinaryMeshAttrributes.h"
#include "../creators/buildingCreator.h"
#include "../creators/groundCreator.h"

#include "math/meshAlgs/Triangulation.h"
#include "math/meshAlgs/MeshRendering.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <Windows.h>
#include "constants.h"
#include <filesystem>
#include <algorithm>
#include <execution>

#include <chrono>



const std::string outputDir = TERRAIN_DIR;//R"(./terrain/chunkMeshes/)";
const std::string attrOutputDir = TERRAIN_ATTR_DIR;//R"(./terrain/chunkMeshes/)";

GenerationSystem::GenerationSystem(std::vector<Box>&& chunks)
    : osmFetcher(),
    chunks(chunks), 
    creators({new groundCreator(),new buildingCreator()})
{

}

void GenerationSystem::generate(bool onlyUseOSMCash)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    //for (Box& chunk : chunks) {
    std::for_each(std::execution::par, chunks.begin(), chunks.end(), [this,onlyUseOSMCash](Box& chunk) {
        auto file = outputDir + chunk.toString() + ".bmesh";
        auto attrFile = attrOutputDir + chunk.toString() + ".bmattr";
        try {

            if (std::filesystem::exists(file)) {
                printf("skipping an already saved chunk chunk\n");
                return;
            }

            BinaryMeshAttrributes binaryAttributes{};
            BinaryMeshSeirilizer::Mesh mesh;

            mesh.attributes = &binaryAttributes;

            printf("going to get Osm for a chunk\n");
            osm::osm osmData = osmFetcher.fetchChunk(chunk,onlyUseOSMCash);


            printf("Got Osm for a chunk\n");

            for (icreator* creator : creators)
                creator->createInto(mesh, osmData, chunk);


            BinaryMeshSeirilizer binaryMesh(mesh);


            // write to file
            printf("writing to mesh file\n");
            {

                std::ofstream out;
                out.open(file, std::fstream::out | std::fstream::binary);

                out.write(reinterpret_cast<char*>(binaryMesh.mesh), binaryMesh.meshLength);
                out.close();

                binaryAttributes.saveTo(attrFile );

                printf("wrote to mesh file\n");
            }
        }
        catch (...) {
            return;
        }
    });
    auto endTime = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double>(endTime - startTime);
    printf("finished all chunks in %f seconds \n",time);
}

void GenerationSystem::debugChunk(size_t index)
{
    const auto& chunk = chunks[index];

    BinaryMeshAttrributes binaryAttributes{};
    BinaryMeshSeirilizer::Mesh mesh;

    mesh.attributes = &binaryAttributes;

    printf("going to get Osm for a chunk\n");
    osm::osm osmData = osmFetcher.fetchChunk(chunk, false);


    printf("Got Osm for a chunk\n");

    for (icreator* creator : creators)
        creator->createInto(mesh, osmData, chunk);
    //mesh.indicies.erase(mesh.indicies.begin());
    meshAlgs::displayMesh(mesh);
    

    //BinaryMeshSeirilizer binaryMesh(mesh);

}

std::vector<Box> GenerationSystem::genreateChunksAround(glm::dvec2 desired, int divided, glm::ivec2 formation)
{
    auto chunk = actualChunk(desired, divided);

    std::vector<Box> chunks;

    auto minx = static_cast<int>(floor(static_cast<double>(formation.x) / 2));
    auto miny = static_cast<int>(floor(static_cast<double>(formation.y) / 2));

    for (int x = -minx; x <= minx; x++)
    {
        for (int y = -miny; y <= miny; y++)
        {
            auto stride = glm::dvec2(chunk.size.x * static_cast<double>(x), chunk.size.y * static_cast<double>(y));
            auto newStart = chunk.start + stride;

            auto newChunk = Box(newStart, chunk.size);
            chunks.push_back(newChunk);
        }
    }
    
    return chunks;

}

Box GenerationSystem::actualChunk(glm::dvec2 desired, int divided)
{
    auto frame = Box(glm::dvec2(0, -90), glm::dvec2(90, 90));

    for (size_t i = 0; i < divided; i++)
    {
        frame.size /= 2;
    }

    auto x1 = floor((desired.x - 00) / frame.size.x) * frame.size.x;
    auto y1 = floor((desired.y + 00) / frame.size.y) * frame.size.y;

    return Box(glm::dvec2(x1, y1),frame.size);
}
