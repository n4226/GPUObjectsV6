#include "OsmFetcher.h"


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <fstream>

#include <stdio.h>

#include "dependencies/httplib.h"
#include "constants.h"


const std::string cashDir = OSM_CASH_DIR;//R"(.\osmCash\)";

#define SERVER_LOCAL 1

#if SERVER_LOCAL
const char* server = "http://localhost";
#else
 //const char* server = "http://overpass-api.de";
#endif
OsmFetcher::OsmFetcher()
{
}

osm::osm OsmFetcher::fetchChunk(Box frame, bool onlyUseOSMCash)
{
#if !SERVER_LOCAL
    auto ticket = waitforServerQueue.take();
#endif

    auto file = cashDir + frame.toString() + ".osm";

    // check if in local cash if so return that
    printf("attempting to open osm cash file\n");
    std::ifstream f(file);
    if (f.good()) {
        printf("using osm cash to load osm\n");
        std::string str((std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());
        if (str.substr(0, 5) == "<?xml") {
            remove(file.c_str());
            throw std::runtime_error("osm data not json");
        }
        return osm::    makeOSM(str);
    }
    else if (onlyUseOSMCash)
        throw std::runtime_error("not in osm cash");
#if !SERVER_LOCAL
    ticket.wait();
    defer(ticket.done());
#endif
    printf("fetching osm from server\n");
    //get from server 

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


    if (server != "http://localhost") {
        printf("sleeping with json data\n");
        Sleep(8000);
    }
    else {
        printf("not sleeping\n");
    }

    return parsedOsm;
}


std::string OsmFetcher::getQuery(Box frame)
{
    // full osm data
    const char* str = R"(
        <osm-script output="json" output-config="" timeout="25">
        <union into="_">
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        <query into="_" type="way">
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        <query into="_" type="relation">
        <bbox-query s="%f" w="%f" n="%f" e="%f"/>
        </query>
        </union>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="body" n="" order="id" s="" w=""/>
        <recurse from="_" into="_" type="down"/>
        <print e="" from="_" geometry="skeleton" ids="yes" limit="" mode="skeleton" n="" order="quadtile" s="" w=""/>
        </osm-script>
        )";
    // old only buildings
  /*  const char* str = R"(
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
        )";*/

    char buffer[1'000];

    sprintf_s(buffer, str, frame.start.x, frame.start.y, frame.getEnd().x, frame.getEnd().y, frame.start.x, frame.start.y, frame.getEnd().x, frame.getEnd().y, frame.start.x, frame.start.y, frame.getEnd().x, frame.getEnd().y);

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
