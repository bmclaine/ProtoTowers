#pragma once

#include <vector>
#include "ObjectData.h"

class Scene{

public:
	Scene() = default;
	~Scene(){
		for (unsigned int i = 0; i < objects.size(); ++i){
			delete objects[i];
		}
		objects.clear();
	}

	std::vector<ObjectData*> objects;
};