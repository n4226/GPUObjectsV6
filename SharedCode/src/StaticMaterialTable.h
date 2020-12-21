#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <map>

class StaticMaterialTable
{

public:
	static std::unordered_map<std::string, glm::uint32> entries;
	static std::map<glm::uint32, std::string> reverseEntries;
};

