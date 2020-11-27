#include "OsmFetcher.h"


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>


#include "../dependencies/httplib.h"

OsmFetcher::OsmFetcher()
{
}

osm::osm OsmFetcher::fetchChunk(Box frame)
{

    // check if in local cash if so return that

    //get from server 

    const char* server = "http://overpass-api.de";;
    httplib::Client cli(server);//();

    auto str = getQuery(frame);

    auto response = cli.Post("/api/interpreter", str, "application/xml");
    
    //std::cout << response->body << "\n";

    // pars to osm

    osm::osm parsedOsm = osm::makeOSM(response->body);

    //update cash


    return osm::osm();
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
