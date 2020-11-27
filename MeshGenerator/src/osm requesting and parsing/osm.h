#pragma once

#include <string>
#include <vector>
#include <memory>

namespace osm {

    enum class role : int { inner, outer };

    enum class type : int { node, relation, way };

    struct member {
        type type;
        int64_t ref;
        role role;
    };

    struct tag {
        std::string key;
        std::string value;
    };

    struct element {
        type type;
        int64_t id;
        std::shared_ptr<std::vector<int64_t>> nodes;
        std::vector<tag> tags;
        std::shared_ptr<std::vector<member>> members;
        std::shared_ptr<double> lat;
        std::shared_ptr<double> lon;
    };

    struct osm3S {
        std::string timestampOsmBase;
        std::string copyright;
    };

    struct osm {
        double version;
        std::string generator;
        osm3S osm3S;
        std::vector<element> elements;
    };

    osm makeOSM(std::string& str);

}



