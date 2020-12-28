#include "osm.h"

//#include "nlohmann/json.hpp"

#include <execution>
#include <ranges>
#include <algorithm>


// rapidjson/example/simpledom/simpledom.cpp`
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"



namespace osm {
	//using namespace nlohmann;
	using namespace rapidjson;

	osm makeOSM(std::string& str)
	{
		if (str == "" || str.substr(0, 5) == "<?xml") {
			throw std::runtime_error("not json");
		}
		Document j;

		j.Parse(str.c_str());

		//auto j = json::parse(str);

		osm osm;

		// parse headers

		osm.version = j["version"].GetDouble();
		osm.generator = j["generator"].GetString();
		osm.osm3S.timestampOsmBase = j["osm3s"]["timestamp_osm_base"].GetString();
		osm.osm3S.copyright = j["osm3s"]["copyright"].GetString();

		// parse elements
		

		auto jElements = j["elements"].GetArray();
		osm.elements.resize(jElements.Size());
		std::transform(std::execution::seq, jElements.begin(), jElements.end(), osm.elements.begin(), [](Value& je) {
			element e;

			e.id = je["id"].GetInt64();

			if (je.HasMember("nodes")) {
				auto nodes = je["nodes"].GetArray();
				e.nodes.resize(nodes.Size());
				for (size_t i = 0; i < nodes.Size(); i++)
					e.nodes[i] = nodes[i].GetUint64();
			}

			if (je.HasMember("members")) {
				auto members = je["members"].GetArray();
				e.members = std::make_shared<std::vector<member>>();
				std::vector<member>& eMembers = *e.members;
				eMembers.resize(members.Size());
				for (size_t i = 0; i < members.Size(); i++) {

					eMembers[i].ref = members[i]["ref"].GetInt64();

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

			if (je.HasMember("tags")) {
				auto tags = je["tags"].GetObject();

				e.tags = std::map<std::string, std::string>();
				
				for (auto& t : tags) {
					e.tags.insert(std::pair<std::string, std::string>(t.name.GetString(), t.value.GetString()));
				}
			}


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


			if (je.HasMember("lat")) {
				e.lat = std::make_shared<double>();
				*e.lat = je["lat"].GetDouble();
			}
			if (je.HasMember("lon")) {
				e.lon = std::make_shared<double>();
				*e.lon = je["lon"].GetDouble();
			}
			return e;
		});

		/*for (size_t ei = 0; ei < j["elements"].size(); ei++)
		{
			auto je = j["elements"][ei];

			
		}*/

		printf("osm parsed");
		osm.createCacheStructure();
		return osm;
	}


	void osm::createCacheStructure()
	{
		for (element& e : elements) {
			elementMap[e.id] = &e;
		}
	}

	std::vector<element*> osm::nodesIn(element e)
	{
		if (e.nodes.size() == 0) return {};

		std::vector<element*> results;
		results.resize(e.nodes.size());

		std::transform(e.nodes.begin(), e.nodes.end(), results.begin(), [&](int64_t id) {
			auto result = elementMap.find(id)->second;
			return result;
		});
		return results;
	}

}
