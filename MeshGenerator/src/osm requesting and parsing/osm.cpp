#include "osm.h"

#include "nlohmann/json.hpp"

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
		
		for (size_t i = 0; i < j["elements"].size(); i++)
		{
			auto je = j["elements"][i];
			
			element e;

			e.type = 

		}



		return osm;
	}


}
