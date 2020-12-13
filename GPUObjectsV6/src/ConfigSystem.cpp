#include "pch.h"
#include "ConfigSystem.h"

#include <iomanip>

using namespace nlohmann;

const std::string configDir = R"(./config/)";

ConfigSystem configSystem = ConfigSystem();


ConfigSystem::Config& ConfigSystem::global()
{
    // TODO: insert return statement here
    return *config;
}

void ConfigSystem::readFromDisk()
{
    auto filePath = configDir + "global.config";

    std::ifstream f(filePath);
    if (f.good()) {
        std::string str((std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());
        json jData = json::parse(str);
        config = Config::fromJson(jData);
    }
    else {
        resetToDefault();
        writeToDisk();
    }

}

void ConfigSystem::writeToDisk()
{
    auto filePath = configDir + "global.config";

    auto data = config->toJson();

    {
        std::ofstream out;
        out.open(filePath, std::fstream::out);
        //out.open(file, std::fstream::out);
        out << std::setw(4) << data << std::endl;
        out.close();
    }
}

void ConfigSystem::resetToDefault()
{
    if (config != nullptr)
        delete config;

    config = new Config;

    config->windows = {};
    config->windows.resize(1);

    config->windows[0].mode = ConfigSystem::Config::Window::WindowMode::windowed;
    config->windows[0].monitor = 0;
    config->windows[0].size = glm::ivec2(1920,1080);



}

nlohmann::json ConfigSystem::Config::toJson()
{
    json j;

    std::vector<json> jwindows;

    jwindows.resize(windows.size());

    for (size_t i = 0; i < windows.size(); i++)
    {
        jwindows[i]["monitor"] = windows[i].monitor;
        switch (windows[i].mode)
        {
        case ConfigSystem::Config::Window::WindowMode::windowed:

            jwindows[i]["mode"] = "windowed";
            break;
        case ConfigSystem::Config::Window::WindowMode::FullscreenBorderless:

            jwindows[i]["mode"] = "FullscreenBorderless";
            break;
        case ConfigSystem::Config::Window::WindowMode::Fullscreen:

            jwindows[i]["mode"] = "Fullscreen";
            break;
        default:
            break;
        }

        jwindows[i]["size"]["x"] = windows[i].size.x;
        jwindows[i]["size"]["y"] = windows[i].size.y;
    }
    j["windows"] = jwindows;

    return j;
}

ConfigSystem::Config* ConfigSystem::Config::fromJson(nlohmann::json& j)
{
    auto config = new Config();

    auto jwins = j["windows"];
    config->windows.resize(jwins.size());

    for (size_t i = 0; i < jwins.size(); i++)
    {
        config->windows[i].monitor =    jwins[i]["monitor"];

        std::string windowMode = jwins[i]["mode"];

        if (windowMode ==  "Fullscreen")
            config->windows[i].mode = ConfigSystem::Config::Window::WindowMode::Fullscreen;
        else if (windowMode == "FullscreenBorderless")
            config->windows[i].mode = ConfigSystem::Config::Window::WindowMode::FullscreenBorderless;
        else //if (windowMode == "windowed")
            config->windows[i].mode = ConfigSystem::Config::Window::WindowMode::windowed;

        config->windows[i].size.x =     jwins[i]["size"]["x"];
        config->windows[i].size.y =     jwins[i]["size"]["y"];
    }

    return config;
}
