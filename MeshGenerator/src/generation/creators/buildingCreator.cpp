#include "buildingCreator.h"

#include <algorithm>

#include "math/meshAlgs/Triangulation.h"

void buildingCreator::createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame, int lod)
{
	mesh.indicies.push_back({});
	mesh.attributes->subMeshMats.push_back(2);
	
	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one 
	// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land
	//
	for (osm::element& element : osm.elements) {
		if (element.type == osm::type::way && element.tags.count("building") > 0) {
			auto nodes = osm.nodesIn(element);

			// first node IS duplicated - AT INDEX 0 AND INDEX SIZE - 1
			std::vector<glm::dvec2> basePath(nodes.size());

			std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element* element) {
				return glm::dvec2(*element->lat, *element->lon);
			});
			auto bounds = new Box();
			*bounds = meshAlgs::bounds(basePath);
			buldingAABBS.emplace(&element, bounds);
		}
	}



	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one loop
	for (osm::element& element : osm.elements) {
		
		// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land



		if (element.type == osm::type::way && element.tags.count("building") > 0) {
			addBuilding(mesh, osm, element, frame,lod);
		}

	}


}

void buildingCreator::addBuilding(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, osm::element& building, const Box& frame, int lod)
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
	else height = 4;

	auto startVertOfset = mesh.verts.size();

	auto nodes = osm.nodesIn(building);

	// first node IS duplicated - AT INDEX 0 AND INDEX SIZE - 1
	std::vector<glm::dvec2> basePath(nodes.size());

	std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&](osm::element* element) {
		auto posLatLon = glm::dvec2(*element->lat, *element->lon);

		/*posLatLon.x = glm::max(posLatLon.x, frame.start.x);
		posLatLon.y = glm::max(posLatLon.y, frame.start.y);
		posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
		posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);*/

		return posLatLon;
	});

	auto bounds = meshAlgs::bounds(basePath);

	glm::dvec2 min = bounds.start;
	glm::dvec2 max = bounds.getEnd();


	if (!frame.contains(min) && !frame.contains(max))
		return;



	//for (osm::element& element : osm.elements) {
	//	if (element.type == osm::type::way && element.tags.count("building") > 0) {
	//		
	//		if (buldingAABBS.count(&element) > 0) {
	//			auto other_bounds = buldingAABBS.at(&element);

	//			if (lod > 0 && (bounds.containsAny(other_bounds->polygon()) || other_bounds->containsAny(bounds.polygon()))) {
	//				return;
	//			}
	//		}
	//	}
	//}



	// roof
	// creates a po.ygon with one set of points, the base points dropping the last point since it is just the first point
	std::vector<std::vector<glm::dvec2>> roofPoly = { std::vector<glm::dvec2>(basePath.begin(),std::prev(basePath.end())) };

	auto [roofMesh, isCLockWise] = meshAlgs::triangulate(roofPoly);

	//TODO --- deal with duplicate verticies her this is important

	startVertOfset = mesh.verts.size();

	for (size_t i = 0; i < roofMesh->verts.size(); i++)
	{
		auto posLatLon = roofMesh->verts[i];
		auto posLLA = glm::dvec3(posLatLon.x, posLatLon.y, height);
		auto pos1 = Math::LlatoGeo(posLLA, glm::dvec3(0), radius) - center_geo;
		mesh.verts.push_back(pos1);

		auto geo_unCentered = Math::LlatoGeo(posLLA, {}, radius);

		auto normal = static_cast<glm::vec3>(glm::normalize(geo_unCentered));

		mesh.normals.push_back(normal);

		mesh.tangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));

		auto uv = glm::vec2(((posLatLon - min) / (max - min)) * Math::llaDistance(min, max));
		mesh.uvs.push_back(uv);
	}

	for (size_t i = 0; i < roofMesh->indicies[0].size(); i++)
	{
		mesh.indicies[mesh.indicies.size() - 1].push_back(roofMesh->indicies[0][i] + startVertOfset);
	}



	// walls 
	for (size_t i = 0; i < (basePath.size() - 1); i++)
	{
		auto localOff = static_cast<uint32_t>(mesh.verts.size());

		auto pos1 = Math::LlatoGeo(glm::dvec3(basePath[i]    , 0), glm::dvec3(0), radius) - center_geo;
		auto pos2 = Math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - center_geo;


		auto pos3 = Math::LlatoGeo(glm::dvec3(basePath[i], height), glm::dvec3(0), radius) - center_geo;
		auto pos4 = Math::LlatoGeo(glm::dvec3(basePath[i + 1], height), glm::dvec3(0), radius) - center_geo;



		auto normal = glm::normalize(glm::cross(pos2 - pos3, pos2 - pos1));

		if (isCLockWise) {
			normal = -normal;
		}

		mesh.verts.push_back(glm::vec3(pos1));
		mesh.verts.push_back(glm::vec3(pos2));
		mesh.verts.push_back(glm::vec3(pos3));
		mesh.verts.push_back(glm::vec3(pos4));

		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));
		mesh.normals.push_back(glm::vec3(normal));

		mesh.tangents.push_back(glm::vec3(0));
		mesh.tangents.push_back(glm::vec3(0));
		mesh.tangents.push_back(glm::vec3(0));
		mesh.tangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));
		mesh.bitangents.push_back(glm::vec3(0));

		// scalled uvs

		mesh.uvs.push_back(glm::vec2(0, 0));
		mesh.uvs.push_back(glm::vec2(glm::distance(pos2, pos1), 0));
		mesh.uvs.push_back(glm::vec2(0, height));
		mesh.uvs.push_back(glm::vec2(glm::distance(pos2, pos1), height));


		//mesh.uvs.push_back(glm::vec2(0,0));
		//mesh.uvs.push_back(glm::vec2(1,0));
		//mesh.uvs.push_back(glm::vec2(0,1));
		//mesh.uvs.push_back(glm::vec2(1,1));

		//TODO: later add wall inside detection

		if (!isCLockWise) {
			// wall side 1
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 0);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 3);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
		}
		else {
			// wall side 2
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 0);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 2);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 3);
			mesh.indicies[mesh.indicies.size() - 1].push_back(localOff + 1);
		}
	}

}
