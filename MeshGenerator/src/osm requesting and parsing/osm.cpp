#include "osm.h"

#include "nlohmann/json.hpp"

#include <execution>
#include <ranges>
#include <algorithm>

namespace osm {
	using namespace nlohmann;
	osm makeOSM(std::string& str)
	{
		auto j = json::parse(str);

		osm osm;

		// parse headers

		osm.version = j["version"];
		osm.generator = j["generator"];
		osm.osm3S.timestampOsmBase = j["osm3s"]["timestamp_osm_base"];
		osm.osm3S.copyright = j["osm3s"]["copyright"];

		// parse elements
		

		auto jElements = j["elements"];
		osm.elements.resize(jElements.size());
		std::transform(std::execution::par, jElements.begin(), jElements.end(), osm.elements.begin(), [](nlohmann::json je) {
			element e;

			e.id = je["id"];

			auto nodes = je["nodes"];
			if (!nodes.is_null()) {
				e.nodes.resize(nodes.size());
				for (size_t i = 0; i < nodes.size(); i++)
					e.nodes[i] = nodes[i].get<uint64_t>();
			}

			auto members = je["members"];
			if (!members.is_null()) {
				e.members = std::make_shared<std::vector<member>>();
				std::vector<member>& eMembers = *e.members;
				eMembers.resize(members.size());
				for (size_t i = 0; i < members.size(); i++) {

					eMembers[i].ref = members[i]["ref"];

					if (members[i]["role"] == "outer") {
						eMembers[i].role = role::outer;
					}
					else if (members[i]["role"] == "inner") {
						eMembers[i].role = role::inner;

					}
					else {

					}

					if (members[i]["type"] == "way") {
						eMembers[i].type = type::way;
					}
					else if (members[i]["type"] == "node") {
						eMembers[i].type = type::node;

					}
					else if (members[i]["type"] == "relation") {
						eMembers[i].type = type::relation;

					}
					else {

					}

				}
			}

			if (!je["tags"].is_null())
				e.tags = je["tags"].get<std::map<std::string, std::string>>();


			if (je["type"] == "way") {
				e.type = type::way;
			}
			else if (je["type"] == "node") {
				e.type = type::node;

			}
			else if (je["type"] == "relation") {
				e.type = type::relation;

			}
			else {

			}

			auto lat = je["lat"];
			auto lon = je["lon"];

			if (!lat.is_null()) {
				e.lat = std::make_shared<double>();
				*e.lat = lat.get<double>();
			}
			if (!lon.is_null()) {
				e.lon = std::make_shared<double>();
				*e.lon = lon.get<double>();
			}
			return e;
		});

		/*for (size_t ei = 0; ei < j["elements"].size(); ei++)
		{
			auto je = j["elements"][ei];

			
		}*/

		printf("osm parsed");

		return osm;
	}


}
