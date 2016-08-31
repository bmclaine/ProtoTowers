#pragma once

#include "Property.h"
#include "List.h"

struct ObjectData{

	List<Property*> properties;
	std::string name;

	ObjectData(std::string& _name){
		name = _name;
	}

	Property* GetProperty(std::string& propName){
		for (unsigned int i = 0; i < properties.size(); ++i){
			if (properties[i]->name == propName)
				return properties[i];
		}

		return nullptr;
	}

	~ObjectData(){
		for (unsigned int i = 0; i < properties.size(); ++i){
			delete properties[i];
		}
	}

};