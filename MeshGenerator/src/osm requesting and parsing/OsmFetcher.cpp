#include "OsmFetcher.h"


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>

#include "../dependencies/httplib.h"
#include "constants.h"


const std::string cashDir = OSM_CASH_DIR;//R"(.\osmCash\)";

OsmFetcher::OsmFetcher()
{
}

osm::osm OsmFetcher::fetchChunk(Box frame, bool onlyUseOSMCash)
{
    auto file = cashDir + frame.toString() + ".osm";

    // check if in local cash if so return that
    printf("attempting to open osm cash file\n");
    std::ifstream f(file);
    if (f.good()) {
        printf("using osm cash to load osm\n");
        std::string str((std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());
        return osm::makeOSM(str);
    }
    else if (onlyUseOSMCash)
        throw std::runtime_error("not in osm cash");
    printf("fetching osm from server\n");
    //get from server 

    const char* server = "http://overpass-api.de";;
    httplib::Client cli(server);//();

    auto str = getQuery(frame);

    auto response = cli.Post("/api/interpreter", str, "application/xml");
    
    //std::cout << response->body << "\n";

    //update cash - raw osm

    if (response.error() == httplib::Error::Read)
        throw std::runtime_error("error");
    if (response->body.empty())
        throw std::runtime_error("osmEmpty");

    {
        std::ofstream out;
        out.open(file, std::fstream::out);
        //out.open(file, std::fstream::out);
        out << response->body;
        out.close();
    }

    // pars to osm
    printf("parsing osm from json\n");

    osm::osm parsedOsm = osm::makeOSM(response->body);

    printf("sleeping with json data\n");

    Sleep(8000);

    return parsedOsm;
}


std::string OsmFetcher::getQuery(Box frame)
{

    const char* str = R"(
    <osm-script output="json" output-config="" timeout="25">
  <union into="_">
    <query into="_" type="way">
      <has-kv k="building" modv="" v=""/>
      <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        <query into="_" type="relation">
        <has-kv k="building" modv = "" v = ""/>
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        </union>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="body" n="" order="id" s="" w=""/>
        <recurse from="_" into="_" type="down"/>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="skeleton" n="" order="quadtile" s="" w=""/>
        </osm-script>
        )";

    char buffer[1'000];

    sprintf_s(buffer, str, frame.size.x, frame.start.y, frame.getEnd().x, frame.getEnd().y, frame.size.x, frame.start.y, frame.getEnd().x, frame.getEnd().y);

    return std::string(buffer);

}

std::string OsmFetcher::getTestQuery()
{
    return
        R"(
        < osm - script output = "json" output - config = "" timeout = "25" >
        <union into = "_">
        <query into = "_" type = "way">
        <has - kv k = "building" modv = "" v = "" / >
        <bbox - query s = "40.610319941413" w = "-74.039182662964" n = "40.618635305267" e = "-74.027112722397" / >
        < / query>
        <query into = "_" type = "relation">
        <has - kv k = "building" modv = "" v = "" / >
        <bbox - query s = "40.610319941413" w = "-74.039182662964" n = "40.618635305267" e = "-74.027112722397" / >
        < / query>
        < / union>
        <print e = "" from = "_" geometry = "skeleton" ids = "yes" limit = "" mode = "body" n = "" order = "id" s = "" w = "" / >
        <recurse from = "_" into = "_" type = "down" / >
        <print e = "" from = "_" geometry = "skeleton" ids = "yes" limit = "" mode = "skeleton" n = "" order = "quadtile" s = "" w = "" / >
        < / osm - script>
        )";
}
