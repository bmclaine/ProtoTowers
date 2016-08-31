#include "../Public/SceneManager.h"
#include "ObjectManager.h"
#include "FileSystem.h"
#include "Object.h"
#include "GameObject.h"
#include "Assets\Mesh.h"
#include "Components\Transform.h"
#include "../../Interface/Public/Engine.h"

SceneManager::~SceneManager(){
	auto sceneIter = m_Scenes.begin();
	while (sceneIter != m_Scenes.end()){
		delete sceneIter->second;
		++sceneIter;
	}
	m_Scenes.clear();
}

bool SceneManager::LoadScene(std::string& sceneName){

	Inferno::ClearScene();
	Inferno_UI::ClearView();
	Inferno::CleanUpParticles();

	if (m_Scenes.count(sceneName) == 0) {
		std::string errorPre = "Scene <";
		std::string error = "> doesn't exist";
		Inferno::Alert((errorPre + sceneName + error).c_str());
		return false;
	}

	Scene* scene = m_Scenes.find(sceneName)->second;
	if (scene == nullptr) return false;

	Mesh* navMesh = Inferno::GetMeshByName(sceneName.c_str());

	Matrix4 identity = Matrix4::Identity();
	if (navMesh && navMesh->GetSubMeshes()[0].vertexCount > 0)
		Inferno::InitializeNavMesh(navMesh, identity);
	else{
		navMesh = Inferno::GetMeshByName("NavMesh");	
		if (navMesh)
			Inferno::InitializeNavMesh(navMesh, identity);
	}

	std::map<unsigned int, Object*> objectMap;
	std::map<unsigned int, ObjectData*> dataMap;

	unsigned int objectDataCount = (unsigned int)scene->objects.size();
	for (unsigned int i = 0; i < objectDataCount; ++i){
		ObjectData* curObjectData = scene->objects[i];
		Property* guidProperty = curObjectData->GetProperty(std::string("GUID"));
		unsigned int guidValue = (unsigned int)atoi(guidProperty->value.c_str());
		dataMap.emplace(guidValue, curObjectData);
	}

	for (unsigned int i = 0; i < objectDataCount; ++i){

		ObjectData* curObjectData = scene->objects[i];
		if (curObjectData->name != "Transform") continue;

		Property* parentProperty = curObjectData->GetProperty(std::string("Parent"));
		if (parentProperty != nullptr) continue;

		Property* gameObjectProperty = curObjectData->GetProperty(std::string("gameObject"));
		unsigned int gameObjectGUID = (unsigned int)atoi(gameObjectProperty->value.c_str());
		ObjectData* gameObjectData = dataMap.find(gameObjectGUID)->second;
		Property* prefabNameProperty = gameObjectData->GetProperty(std::string("PrefabName"));
		std::string prefabName = prefabNameProperty->value;
		Property* gameObjectNameProperty = gameObjectData->GetProperty(std::string("Name"));
		std::string gameObjectName = gameObjectNameProperty->value;

		if (prefabName == ""){
			int debug = 0;
		}

		GameObject* gameObject = Inferno::Instantiate(prefabName);

		Property* transformProperty = curObjectData->GetProperty(std::string("GUID"));
		unsigned int transformGUID = (unsigned int)atoi(transformProperty->value.c_str());
		gameObject->transform->SetInstanceID(transformGUID);

		SetTransformIds(objectMap, dataMap, gameObject->transform);
	}
	
	for (auto objectMapIter = objectMap.begin(); objectMapIter != objectMap.end(); ++objectMapIter){
		Object* obj = objectMapIter->second;
		obj->PostInit(objectMap, dataMap);
	}

	return true;
}

void SceneManager::SetTransformIds(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap, Transform* rootTransform){

	ObjectData* rootTransformData = dataMap.find(rootTransform->GetInstanceID())->second;
	unsigned int rootTransformGUID = (unsigned int)atoi(rootTransformData->GetProperty(std::string("GUID"))->value.c_str());
	objectMap.emplace(rootTransformGUID, rootTransform);

	unsigned int gameObjectGUID = (unsigned int)atoi(rootTransformData->GetProperty(std::string("gameObject"))->value.c_str());
	rootTransform->gameObject->SetInstanceID(gameObjectGUID);
	objectMap.emplace(gameObjectGUID, rootTransform->gameObject);

	//unsigned int childCount = (unsigned int)atoi(rootTransformData->GetProperty(std::string("ChildCount"))->value.c_str());
	unsigned int childCount = rootTransform->GetChildCount();
	for (unsigned int i = 0; i < childCount; ++i){

		std::string childString = "Child" + std::to_string(i);
		Property* childProperty = rootTransformData->GetProperty(childString);
		if (childProperty == nullptr) break;
		unsigned int childGUID = (unsigned int)atoi(childProperty->value.c_str());

		Transform* childTransform = rootTransform->GetChild(i);
		childTransform->SetInstanceID(childGUID);

		SetTransformIds(objectMap, dataMap, childTransform);
	}

	SetComponentIds(objectMap, dataMap, rootTransform->gameObject);

}

void SceneManager::SetComponentIds(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap, GameObject* gameObject){

	unsigned int instanceID = gameObject->GetInstanceID();
	ObjectData* gameObjectData = dataMap.find(gameObject->GetInstanceID())->second;

	std::vector<IComponent*>& components = gameObject->GetComponents<IComponent>();
	for (unsigned int i = 1; i < (unsigned int)components.size(); ++i){
		std::string componentString = "Component" + std::to_string(i);
		IComponent* component = components[i];
		Property* componentProperty = gameObjectData->GetProperty(componentString);
		unsigned int componentGUID = (unsigned int)atoi(componentProperty->value.c_str());
		component->SetInstanceID(componentGUID);
		objectMap.emplace(componentGUID, component);
	}
}

void SceneManager::Initialize(){

	FileSystem fileSystem;

	std::string sceneExt = ".scene";

	std::vector<std::string> sceneNames;

	std::string& folderPath = fileSystem.GetScenesFolder();

	fileSystem.GetFilesInDirectory(folderPath, sceneExt, sceneNames);

	size_t fileCount = sceneNames.size();

	for (size_t file = 0; file < fileCount; ++file){

		Scene* scene = DeserializeScene(folderPath + sceneNames[file] + sceneExt);

		if (scene == nullptr) continue;

		m_Scenes.emplace(sceneNames[file], scene);
	}
}

Scene* SceneManager::DeserializeScene(std::string& filePath){
	std::ifstream fileStream;
	fileStream.open(filePath.c_str(), std::ios_base::in);

	if (!fileStream.is_open())
		return nullptr;

	Scene* scene = new Scene();

	std::string data = "";

	while (true){
		std::getline(fileStream, data);
		if (data == "<end>") break;

		std::string objectName = "";

		for (unsigned int i = 0; i < data.size(); ++i){
			if (data[i] == '<') break;

			objectName += data[i];
		}

		ObjectData* objectData = new ObjectData(objectName);
		DeserializeObject(objectData, fileStream);
		scene->objects.push_back(objectData);
	}

	return scene;
}

void SceneManager::DeserializeObject(ObjectData* objectData, std::ifstream& fileStream){
	std::string data = "";

	while (true){
		std::getline(fileStream, data);
		if (data == objectData->name + "/>")
			break;

		std::string propertyName = "";
		int i = 0;
		for (; i < data.size(); ++i){
			if (data[i] == '>')
				break;

			if (data[i] != '\t' && data[i] != '<' && data[i] != '>')
				propertyName += data[i];
		}

		++i;
		std::string value = "";
		for (; i < data.size(); ++i){
			if (data[i] == '<')
				break;

			if (data[i] != '\t')
				value += data[i];
		}

		Property* prop = new Property(propertyName, value);
		objectData->properties.add(prop);
	}
}