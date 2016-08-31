#pragma once

#include <fstream>
#include <vector>
#include "Assets\Material.h"
#include <unordered_map>

class MaterialLoader{

public:
	MaterialLoader() = default;
	~MaterialLoader() = default;

	bool LoadMaterials(std::unordered_map<std::string, Material>& materialMap, std::string& filePath);
};