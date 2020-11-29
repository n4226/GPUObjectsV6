#include "buildingCreator.h"

#include <algorithm>

void buildingCreator::createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame)
{
	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one loop
	for (osm::element& element : osm.elements) {
		
		// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land

		if (element.type == osm::type::way && element.tags.count("building") > 0) {
			addBuilding(mesh, osm, element, frame);
		}

	}


}

void buildingCreator::addBuilding(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, osm::element& building, const Box& frame)
{
	constexpr double radius = Math::dEarthRad;
	const glm::dvec3 center_geo = Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);
	double height;
	
	if (building.tags.count("height")) {
		try {
			height = std::stod(building.tags.at("height"));
		}
		catch (...) { height = 4; }
	}

	auto startVertOfset = mesh.verts.size();

	auto nodes = osm.nodesIn(building);

	std::vector<glm::dvec2> basePath(nodes.size());

	std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element& element) {
		return glm::dvec2(element.lat, element.lon);
	});

	for (size_t i = 0; i < (basePath.size() - 1); i++)
	{
		auto localOff = static_cast<uint32_t>(mesh.verts.size());

		auto pos1 = Math::LlatoGeo(glm::dvec3(basePath[i], 0), glm::dvec3(0), radius) - center_geo;
		auto pos2 = Math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - center_geo;


		auto pos3 = Math::LlatoGeo(glm::dvec3(basePath[i], height), glm::dvec3(0), radius) - center_geo;
		auto pos4 = Math::LlatoGeo(glm::dvec3(basePath[i + 1], height), glm::dvec3(0), radius) - center_geo;

		//normal next

		auto normal = glm::normalize(glm::cross(pos2 - pos3, pos2 - pos1));
	
		
		mesh.verts.push_back(glm::vec3(pos1));
		mesh.verts.push_back(glm::vec3(pos2));
		mesh.verts.push_back(glm::vec3(pos3));
		mesh.verts.push_back(glm::vec3(pos4));

		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));

		mesh.uvs.push_back(glm::vec2(0,0));
		mesh.uvs.push_back(glm::vec2(1,0));
		mesh.uvs.push_back(glm::vec2(0,1));
		mesh.uvs.push_back(glm::vec2(1,1));
	
		//TODO: later add wall inside detection

		// wall side 1
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 0);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 3);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);

		// wall side 2
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 0);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 3);
		mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
	}

}
