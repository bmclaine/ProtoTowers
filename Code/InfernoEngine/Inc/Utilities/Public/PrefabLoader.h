#pragma once
#include <string>
#include "List.h"
#include "ObjectData.h"

#include <fstream>

class Prefab{
public:

	enum PrefabType {
		GameObject,
		UIElement
	};

	int allocationAmount;
	PrefabType prefabType;
	List<ObjectData*> objects;

	Prefab(){
		allocationAmount = 1;
		prefabType = PrefabType::GameObject;
	}

	~Prefab(){
		for (unsigned int i = 0; i < objects.size(); ++i){
			delete objects[i];
		}
		objects.clear();
	}
};

struct PrefabAttribute{
	std::string name;
	std::string value;

	PrefabAttribute(std::string& _name, std::string& _value){
		name = _name;
		value = _value;
	}
};

struct PrefabNode{
	std::string name;
	List<PrefabAttribute*> attributes;

	PrefabNode(std::string& _name){
		name = _name;
	}

	~PrefabNode(){
		for (unsigned int i = 0; i < attributes.size(); ++i){
			delete attributes[i];
		}

		attributes.clear();
	}

	void AddAttribute(PrefabAttribute* attribute){
		attributes.add(attribute);
	}

	void RemoveAttribute(std::string& _name){
		for (unsigned int i = 0; i < attributes.size(); ++i){
			if (attributes[i]->name == _name){
				delete attributes[i];
				attributes.removeAt(i);
				break;
			}
		}
	}

	PrefabAttribute* GetAttribute(std::string& _name){
		for (unsigned int i = 0; i < attributes.size(); ++i){
			if (attributes[i]->name == _name)
				return attributes[i];
		}

		return nullptr;
	}

	PrefabAttribute* GetAttribute(int index){
		return attributes[index];
	}
};

struct PrefabRoot{
	std::string name;
	List<PrefabNode*> nodes;

	PrefabRoot(std::string& _name){
		name = _name;
	}

	~PrefabRoot(){
		for (unsigned int i = 0; i < nodes.size(); ++i){
			delete nodes[i];
		}
		nodes.clear();
	}

	void AddNode(PrefabNode* node){
		nodes.add(node);
	}

	void RemoveNode(std::string& nodeName){
		for (unsigned int i = 0; i < nodes.size(); ++i){
			if (nodes[i]->name == nodeName){
				delete nodes[i];
				nodes.removeAt(i);
				break;
			}
		}
	}

	PrefabNode* GetNode(std::string& nodeName){
		for (unsigned int i = 0; i < nodes.size(); ++i){
			if (nodes[i]->name == nodeName)
				return nodes[i];
		}

		return nullptr;
	}

	PrefabNode* GetNode(int index){
		return nodes[index];
	}

	Prefab* CreatePrefab(Prefab::PrefabType prefabType, int allocationAmount = 1){
		if (allocationAmount < 1)
			allocationAmount = 1;

		Prefab* prefab = new Prefab();
		prefab->prefabType = prefabType;
		prefab->allocationAmount = allocationAmount;

		for (unsigned int i = 0; i < nodes.size(); ++i){
			PrefabNode* node = nodes[i];
			ObjectData* data = new ObjectData(node->name);
			for (unsigned int j = 0; j < node->attributes.size(); ++j){
				PrefabAttribute* attribute = node->GetAttribute(j);
				Property* _property = new Property(attribute->name, attribute->value);
				data->properties.add(_property);
			}
			prefab->objects.add(data);
		}

		return prefab;
	}
};


class PrefabLoader{

public:
	PrefabLoader() = default;
	~PrefabLoader() = default;

	Prefab* LoadPrefab(std::string prefabName);

private:
	void DeserializeNode(PrefabNode* node, std::ifstream& prefabFile);
};