#pragma once

#include "pch.h"
#include <nlohmann/json.hpp>



class ConfigSystem
{
public:
	struct Config
	{
		nlohmann::json toJson();
		static Config* fromJson(nlohmann::json& j);

		struct Window {

			enum WindowMode: int
			{
				windowed,FullscreenBorderless,Fullscreen
			};

			int monitor;
			WindowMode mode;
			glm::ivec2 size;
		};

		std::vector<Window> windows;

	};

	Config& global();

	void readFromDisk();
	void writeToDisk();
	void resetToDefault();

private:
	Config* config;
};


extern ConfigSystem configSystem;