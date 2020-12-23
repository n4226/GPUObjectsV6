#include "pch.h"
#include "StaticMaterialTable.h"




std::unordered_map<std::string, glm::uint32> StaticMaterialTable::entries = {

		{"grass1", 0},
		{"building1", 1},

};

std::map<glm::uint32, std::string> StaticMaterialTable::reverseEntries = {

		{0, "grass1"},
		{1, "building1"},

};