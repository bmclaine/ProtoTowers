//Author: William Lewis
//
//Handles loading and switching scenes
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "Scene.h"

class Object;
class Transform;
class GameObject;

class SceneManager {

private:
	std::map<std::string, Scene*> m_Scenes;

public:
	SceneManager() = default;
	~SceneManager();

	void Initialize();
	bool LoadScene(std::string& sceneName);

private:
	Scene* DeserializeScene(std::string& filePath);
	void DeserializeObject(ObjectData* objectData, std::ifstream& fileStream);
	void SetTransformIds(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap, Transform* rootTransform);
	void SetComponentIds(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap, GameObject* gameObject);

};

