#include "PrefabLoader.h"

Prefab* PrefabLoader::LoadPrefab(std::string prefabName){
	std::ifstream prefabFile;
	prefabFile.open(prefabName.c_str(), std::ios_base::in);

	if (!prefabFile.is_open())
		return nullptr;

	PrefabRoot* root = new PrefabRoot(prefabName);
	Prefab::PrefabType prefabType = Prefab::PrefabType::GameObject;
	int allocationAmount = 1;

	std::string data;

	//Get The prefabType
	std::getline(prefabFile, data);
	prefabType = (Prefab::PrefabType)atoi(data.c_str());

	std::getline(prefabFile, data);

	std::string allocationAmountString = "";
	bool getInt = false;
	for (unsigned int i = 0; i < data.size(); ++i){
		if (!getInt){
			if (data[i] == '<'){
				getInt = true;
			}
		}
		else{
			if (data[i] != '>')
				allocationAmountString += data[i];
			else
				break;
		}	
	}

	allocationAmount = atoi(allocationAmountString.c_str());

	while (true){
		std::getline(prefabFile, data);
		if (data == "<end>") break;

		std::string nodeName = "";
		for (unsigned int i = 0; i < data.size(); ++i){
			if (data[i] == '<') break;

			nodeName += data[i];
		}

		PrefabNode* node = new PrefabNode(nodeName);
		DeserializeNode(node, prefabFile);
		root->AddNode(node);
	}

	prefabFile.close();

	Prefab* prefab = root->CreatePrefab(prefabType, allocationAmount);
	delete root;
	return prefab;
}

void PrefabLoader::DeserializeNode(PrefabNode* node, std::ifstream& prefabFile){
	std::string data = "";
	while (true){
		std::getline(prefabFile, data);
		if (data == node->name + "/>")
			break;

		std::string attributeName = "";
		int i = 0;
		for (; i < data.size(); ++i){
			if (data[i] == '>')
				break;
			
			if (data[i] != '\t' && data[i] != '<' && data[i] != '>')
				attributeName += data[i];
		}
		++i;
		std::string value = "";
		for (; i < data.size(); ++i){
			if (data[i] == '<')
				break;

			if (data[i] != '\t')
				value += data[i];
		}

		PrefabAttribute* attribute = new PrefabAttribute(attributeName, value);
		node->AddAttribute(attribute);
	}
}

