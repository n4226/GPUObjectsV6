#include "groundCreator.h"

#include "math/Math.h"

#include "../ShapeFileSystem.h"
#include "math/meshAlgs/Triangulation.h"

constexpr double radius = Math::dEarthRad;

void groundCreator::createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame, int lod)
{


    auto lands = createLandPolygonChunkMesh(mesh, frame);

    if (lands->size() == 0)
        createSubdividedQuadChunkMesh(mesh, frame);

    auto oceanMass = *lands;

    //oceanMass.insert(oceanMass.begin(), frame.polygon());
    //oceanMass.insert(oceanMass.begin(), frame.polygon());

    //oceanMass.erase(std::prev(oceanMass.end()));

    //mesh.indicies.push_back({});
    //mesh.attributes->subMeshMats.push_back(0);

    //drawMultPolygonInChunk(oceanMass,mesh,frame);

}

std::vector<std::vector<glm::dvec2>>* groundCreator::createLandPolygonChunkMesh(BinaryMeshSeirilizer::Mesh& mesh, const Box& frame)
{
    mesh.indicies.push_back({});
    mesh.attributes->subMeshMats.push_back(0);


    auto landPolygonsTruncated = new std::vector<std::vector<glm::dvec2>>();

    for (size_t i = 0; i < shapeFileSystem->polygons.size(); i++)
    {

        const auto& bounds = shapeFileSystem->polygonBounds[i];//meshAlgs::bounds(poly.verts);
        auto boundPath = bounds.polygon();//meshAlgs::bounds(poly.verts);
        auto& poly = *shapeFileSystem->polygons[i];

        if (!frame.containsAny(poly.verts)) {
            continue;
        }

        std::vector<glm::dvec2> polyPoints = { poly.verts.begin(),std::prev(poly.verts.end()) };

        if (poly.verts[0] == poly.verts[poly.verts.size() - 1]) {

        }





        //auto cutPolyVerts = meshAlgs::intersectionOf(polyPoints, boundPath);
        std::vector<std::vector<glm::dvec2>> polyVerts = { polyPoints };

        //TODO: need to use boolean ops eventually becuae right now there are many many many extra verticies
        for (size_t i = 0; i < polyVerts[0].size(); i++)
        {
            auto& posLatLon = polyVerts[0][i];

            posLatLon.x = glm::max(posLatLon.x, frame.start.x);
            posLatLon.y = glm::max(posLatLon.y, frame.start.y);
            posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
            posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);
        }

        landPolygonsTruncated->push_back(polyVerts[0]);

        //auto roofMesh = meshAlgs::triangulate(*cutPolyVerts).first;
        
        drawMultPolygonInChunk(polyVerts,mesh,frame);

    }

    return landPolygonsTruncated;
}


void groundCreator::drawMultPolygonInChunk(std::vector<std::vector<glm::dvec2>>& polygon, BinaryMeshSeirilizer::Mesh& mesh, const Box& frame)
{
    auto roofMesh = meshAlgs::triangulate(polygon).first;

    const glm::dvec3 center_geo = Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);
    auto startVertOfset = mesh.verts.size();


    for (size_t i = 0; i < roofMesh->verts.size(); i++)
    {
        auto posLatLon = roofMesh->verts[i];

        auto posLLA = glm::dvec3(posLatLon.x, posLatLon.y, 0);
        auto pos1 = Math::LlatoGeo(posLLA, glm::dvec3(0), radius) - center_geo;
        mesh.verts.push_back(pos1);

        auto geo_unCentered = Math::LlatoGeo(posLLA, {}, radius);

        auto normal = static_cast<glm::vec3>(glm::normalize(geo_unCentered));

        mesh.normals.push_back(normal);

        mesh.tangents.push_back(glm::vec3(0));
        mesh.bitangents.push_back(glm::vec3(0));

        auto uv = glm::vec2(((posLatLon - frame.start) / (frame.getEnd() - frame.start)) * Math::llaDistance(frame.start, frame.getEnd()));

        mesh.uvs.push_back(uv);
    }

    for (size_t i = 0; i < roofMesh->indicies[0].size(); i++)
    {
        mesh.indicies[mesh.indicies.size() - 1].push_back(roofMesh->indicies[0][i] + startVertOfset);
    }

}


//TODO: right now this assumes it is the first creator to act on the mesh
void groundCreator::createSubdividedQuadChunkMesh(BinaryMeshSeirilizer::Mesh & mesh, const Box& frame)
{
    mesh.indicies.push_back({});
    mesh.attributes->subMeshMats.push_back(0);
        

    const glm::dvec3 center_geo = Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);

    size_t resolution = 10;
    auto d_resolution = static_cast<double>(resolution);
    //const Box& frame = frame;

    glm::uint32 vert = 0;
    const glm::uint32 startVertOfset = 0;


    auto nonLLAFrameWidth =  Math::llaDistance(frame.start,glm::vec2(frame.start.x,frame.start.y + frame.size.y));
    auto nonLLAFrameHeight = Math::llaDistance(frame.start,glm::vec2(frame.start.x + frame.size.x,frame.start.y));
    
            
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

            auto geo_unCentered = Math::LlatoGeo(lla, {}, radius);

            mesh.verts.emplace_back(geo_unCentered - center_geo);//Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius));
#if SSSS && _DEBUG
            mesh.normals.emplace_back(glm::vec3(0, 0.8, 0.1));//static_cast<glm::vec3>(glm::normalize(geo_unCentered)) * (static_cast<float>(chunk.lodLevel + 1) / 13.f));
#else
            mesh.normals.emplace_back(static_cast<glm::vec3>(glm::normalize(geo_unCentered)));
#endif


            // chunk uvs    
            auto uvx = (chunkStrideLat / frame.size.x) * nonLLAFrameHeight;
            auto uvy = (chunkStrideLon / frame.size.y) * nonLLAFrameWidth;
            // world uvs
//                    let uvx = (y.double - frame.origin.y) / resolution.double / (360 / frame.size.y) + ((frame.origin.y + 180) / 360)
//                    let uvy = (x.double - frame.origin.x) / resolution.double / (180 / frame.size.x) + ((frame.origin.x + 90) / 180)
            // world uvs 2
//                    let uvy = ((lla.x + 90) / 180)
//                    let uvx = ((lla.y + 180) / 360)
//                    if uvx > 1 {
//                        uvx -= 1
//                    }

            mesh.uvs.emplace_back(static_cast<float>(uvx), static_cast<float>(uvy));

        }
    }

    for (size_t x = 0; x < resolution; x++)
    {
        for (size_t y = 0; y < resolution; y++)
        {
            // indicies creation

            auto xSize = resolution;

            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + xSize + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + xSize + 1);
            mesh.indicies[mesh.indicies.size() - 1].emplace_back(vert + startVertOfset + xSize + 2);

            vert++;
        }
        vert++;
    }

    mesh.tangents.resize(mesh.verts.size());
    mesh.bitangents.resize(mesh.verts.size());

}
