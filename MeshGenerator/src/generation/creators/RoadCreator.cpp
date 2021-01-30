#include "RoadCreator.h"



void RoadCreator::createInto(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, const Box& frame, int lod)
{

	mesh.indicies.push_back({});
	mesh.attributes->subMeshMats.push_back(1);

	//TODO: somehow prevent each creator from ahving to loop through all the elements in the osm data after eachother by combining them into one 
	// TODO remove duplicate pointes if they fall on a streight 2d line after being clipped to the chunk -> this could be for objecy such as buildings or more commanly to grounds of different surfaces eg ocean and land
	//
	for (osm::element& element : osm.elements) {
		if (element.type == osm::type::way && element.tags.count("highway") > 0) {
			
			addRoad(mesh, osm, element, frame, lod);

		}
	}

}

void RoadCreator::addRoad(BinaryMeshSeirilizer::Mesh& mesh, osm::osm& osm, osm::element& road, const Box& frame, int lod)
{

	constexpr double radius = Math::dEarthRad;
	const glm::dvec3 center_geo = Math::LlatoGeo(glm::dvec3(frame.getCenter(), 0), {}, radius);

	auto startVertOfset = mesh.verts.size();

	auto nodes = osm.nodesIn(road);



	//TODO: make this acurate
	double width;
	double lanes;

	if (road.tags.count("lanes")) {
		try {
			lanes = std::stod(road.tags.at("lanes"));
		}
		catch (...) { lanes = 2; }
	}
	else lanes = 2;

	if (road.tags.count("width")) {
		try {
			width = std::stod(road.tags.at("width"));
		}
		catch (...) { width = lanes * 1; }
	}
	else width = lanes * 1;

	// first node IS duplicated - AT INDEX 0 AND INDEX SIZE - 1
	std::vector<glm::dvec2> basePath(nodes.size());

	std::transform(nodes.begin(), nodes.end(), basePath.begin(), [&, road, basePath,this](osm::element* element) {
		if (element->type != osm::type::node) { 
			auto msg = std::string("tried to get lat lon of non node at: ") + __FILE__ + std::string(" ") + std::string(std::to_string(__LINE__));
			printf(msg.c_str());
			throw std::runtime_error(msg.c_str());
		}
		auto posLatLon = glm::dvec2(*element->lat, *element->lon);

		posLatLon.x = glm::max(posLatLon.x, frame.start.x);
		posLatLon.y = glm::max(posLatLon.y, frame.start.y);
		posLatLon.x = glm::min(posLatLon.x, frame.getEnd().x);
		posLatLon.y = glm::min(posLatLon.y, frame.getEnd().y);

		return posLatLon;
	});

	int vertIndex = 0;

	//TODO: detect if closed path and adjust mesh acordingly
	for (size_t i = 0; i < basePath.size(); i++)
	{

		auto p1 = Math::LlatoGeo(glm::dvec3(basePath[i + 0], 0), glm::dvec3(0), radius) - center_geo;
		//auto p2 = Math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - center_geo;

		auto forward = glm::dvec3(0);

		if (i < basePath.size() - 1) {
			forward += Math::LlatoGeo(glm::dvec3(basePath[i + 1], 0), glm::dvec3(0), radius) - Math::LlatoGeo(glm::dvec3(basePath[i + 0], 0), glm::dvec3(0), radius);
		}
		if (i > 0) {
			forward += Math::LlatoGeo(glm::dvec3(basePath[i + 0], 0), glm::dvec3(0), radius) - Math::LlatoGeo(glm::dvec3(basePath[i - 1], 0), glm::dvec3(0), radius);
		}

		forward = glm::normalize(forward);

		auto normalUP = glm::normalize(center_geo);
		auto left = glm::cross(forward,normalUP);

		auto v1 = p1 + left * width * 0.5;
		auto v2 = p1 - left * width * 0.5;

		mesh.verts.push_back(glm::vec3(v1));
		mesh.verts.push_back(glm::vec3(v2));


		mesh.normals.push_back(glm::vec3(normalUP));
		mesh.normals.push_back(glm::vec3(normalUP));

		mesh.tangents.push_back(glm::vec3(left));
		mesh.tangents.push_back(glm::vec3(left));
		mesh.bitangents.push_back(glm::vec3(forward));
		mesh.bitangents.push_back(glm::vec3(forward));

		// scalled uvs

		mesh.uvs.push_back(glm::vec2(0, 0));
		mesh.uvs.push_back(glm::vec2(width, 0));


		if (i < basePath.size() - 1) {
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 0 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 2 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 1 + startVertOfset);

			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 1 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 2 + startVertOfset);
			mesh.indicies[mesh.indicies.size() - 1].push_back(vertIndex + 3 + startVertOfset);
		}
		vertIndex += 2;
	}


}
