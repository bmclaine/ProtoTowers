#include "MaterialLoader.h"
#include <string>
#include "../../Interface/Public/Engine.h"
#include "GlobalIncludes.h"

bool MaterialLoader::LoadMaterials(std::unordered_map<std::string, Material>& materialMap, std::string& filePath){
	std::ifstream materialFile;
	materialFile.open(filePath.c_str(), std::ios_base::in);

	if (!materialFile.is_open()) return false;

	std::string data = "";

	while (true){
		std::getline(materialFile, data);
		if (data == "<end>" || data.size() == 0)
			break;

		std::string materialName = "";
		if (data == "<end>")
			break;

		int i = 0;
		for (; i < data.size(); ++i) {
			if (data[i] == '>')
				break;

			if (data[i] != '<')
				materialName += data[i];
		}

		if (materialName.size() == 0){
			Inferno::Alert("Invalid material name!!!");
			break;
		}

		std::getline(materialFile, data);

		i = 0;
		for (; i < data.size(); ++i) {
			if (data[i] == '>') {
				++i;
				break;
			}
		}

		std::string diffuseName = "";
		for (; i < data.size(); ++i) {
			diffuseName += data[i];
		}

		std::getline(materialFile, data);

		i = 0;
		for (; i < data.size(); ++i) {
			if (data[i] == '>') {
				++i;
				break;
			}
		}

		std::string normalName = "";
		for (; i < data.size(); ++i) {
			normalName += data[i];
		}

		std::getline(materialFile, data);

		i = 0;
		for (; i < data.size(); ++i) {
			if (data[i] == '>') {
				++i;
				break;
			}
		}

		std::string effectName = "";
		for (; i < data.size(); ++i) {
			effectName += data[i];
		}

		std::getline(materialFile, data);

		Material material;
		material.DiffuseMapID = Inferno::GetImage(Inferno::ToWideString(diffuseName).c_str());
		material.NormalMapID = Inferno::GetImage(Inferno::ToWideString(normalName).c_str());
		material.EffectID = atoi(effectName.c_str());
		materialMap.emplace(materialName, material);
	}

	return true;
}
