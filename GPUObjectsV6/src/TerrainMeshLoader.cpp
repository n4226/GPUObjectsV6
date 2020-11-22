#include "pch.h"
#include "TerrainMeshLoader.h"

Mesh* TerrainMeshLoader::createChunkMesh(const TerrainQuadTreeNode& chunk)
{
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
            mesh->normals.emplace_back(static_cast<glm::vec3>(glm::normalize(geo_unCentered)));

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
