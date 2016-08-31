//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Asset Manager is responsible for handling the
//					loading of all assets.
//*********************************************************************//

#include "AssetManager.h"

#include "FBXLoader.h"
#include "FileSystem.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "MaterialLoader.h"
#include <fstream>

#define DEFAULT_TEXTURE_INDEX -1

AssetManager::AssetManager()
{

}

AssetManager::~AssetManager()
{

}

Mesh* AssetManager::GetMesh(std::string& _meshName)
{
	auto iter = m_umMeshes.find(_meshName);
	if (iter == m_umMeshes.end())
		return nullptr;

	return &iter->second;
}

int AssetManager::GetTextureIndex(std::string& _textureName)
{
	auto iter = m_umTextures.find(_textureName);
	if (iter == m_umTextures.end())
		return -1;

	return iter->second;
}

Material* AssetManager::GetMaterial(std::string& _materialName){

	auto iter = m_umMaterials.find(_materialName);
	if (iter == m_umMaterials.end())
		return nullptr;

	return &iter->second;
}

Animation* AssetManager::GetAnimation(std::string& _animationName)
{
	auto iter = m_umAnimations.find(_animationName);
	if (iter == m_umAnimations.end())
		return nullptr;

	return &iter->second;
}

AnimationSet* AssetManager::GetAnimationSet(std::string& _animSetName)
{
	auto iter = m_umAnimationSets.find(_animSetName);
	if (iter == m_umAnimationSets.end())
		return nullptr;

	return &iter->second;
}

AnimationController* AssetManager::GetAnimationController(std::string& _controllerName)
{
	auto iter = m_umAnimationControllers.find(_controllerName);
	if (iter == m_umAnimationControllers.end())
		return nullptr;

	return &iter->second;
}

void AssetManager::LoadAssets()
{
	LoadMeshes();
	LoadTextures();
	LoadMaterials();
	LoadAnimationSets();
	LoadAnimationControllers();

	// TODO::DELETE	LoadAnimations();
	// ExportLevels();
	// TODO::END
}

void AssetManager::LoadMeshes()
{
	FileSystem fileSystem;
	FBXLoader loader;

	std::string fbxFileExt = ".fbx";
	std::string binFileExt = ".bin";

	std::vector<std::string> fbxMeshes;

	std::string folderPath = fileSystem.GetMeshesFolder();

#if defined(SHIPPING)
	// Get all the file names from the meshes folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, binFileExt, fbxMeshes);
#else
	bool binFileExists = false;

	// Get all the file names from the meshes folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, fbxFileExt, fbxMeshes);
#endif

	size_t fileCount = fbxMeshes.size();
	for (size_t file = 0; file < fileCount; file++)
	{
		Mesh newMesh;
		newMesh.m_Name = fbxMeshes[file];

		// std::vector<std::string> textures;
#if !defined(SHIPPING)
		// Does a Binary File Exist?
		binFileExists = fileSystem.FileExists(folderPath + fbxMeshes[file] + binFileExt);

		// If A binary file already exists AND it is newer than the fbx file..
		if (binFileExists == true && fileSystem.FileIsNewer(folderPath + fbxMeshes[file] + binFileExt, folderPath + fbxMeshes[file] + fbxFileExt) == true)
		{
#endif
			// Read mesh data from the binary file
			loader.LoadMeshFromBinary(folderPath + fbxMeshes[file] + binFileExt, newMesh/*, textures*/);
#if !defined(SHIPPING)
		}
		else
		{
			// Read mesh data from the fbx file
			loader.LoadMeshFromFBX(folderPath + fbxMeshes[file] + fbxFileExt, newMesh/*, textures*/);

			// Write mesh data to a binary file
			loader.WriteMeshToBinary(folderPath + fbxMeshes[file] + binFileExt, newMesh/*, textures*/);
		}
#endif

		// If the new mesh is not already stored, store it in the mesh map
		auto iter = m_umMeshes.find(newMesh.m_Name);
		if (iter == m_umMeshes.end())
			m_umMeshes[newMesh.m_Name] = newMesh;
	}

	// Load all meshes to the graphics manager
	auto iter = m_umMeshes.begin();
	while (iter != m_umMeshes.end())
	{
		//InfernoEngine_Graphics::LoadMesh(&iter->second);
		//if (iter->first != "NavMesh")
		//	iter->second.Finalize();
		//else
		//	iter->second.GetSubMeshes()[0].indiceCount = (unsigned int)iter->second.GetSubMeshes()[0].indices.size();

		InfernoEngine_Graphics::QueueLoadMesh(&iter->second, (iter->first != "NavMesh" && iter->first != "Tutorial"));
		++iter;
	}
}

void AssetManager::LoadTextures()
{
	std::vector<std::string> textureNames;

	FileSystem fileSystem;

	std::string textureFileExt = ".dds";
	std::string cubeMapFileExt = ".dds";

	std::string folderPath = fileSystem.GetTexturesFolder();

	// Get all the file names from the textures folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, textureFileExt, textureNames);
	fileSystem.GetFilesInDirectory(folderPath, cubeMapFileExt, textureNames);

	// If the new texture is not already stored, store it in the texture map
	size_t textureNameCount = textureNames.size();
	for (size_t texture = 0; texture < textureNameCount; texture++)
	{
		m_umTextures[textureNames[texture]] = DEFAULT_TEXTURE_INDEX;
	}

	// Load all textures to the graphics manager
	auto iter = m_umTextures.begin();
	while (iter != m_umTextures.end())
	{
		std::string newText = iter->first + textureFileExt;
		std::wstring textName = Inferno::ToWideString(newText);
		//if (InfernoEngine_Graphics::LoadTexture(textName.c_str()) == -1)
			//InfernoEngine_Graphics::LoadTexture(Inferno::ToWideString(iter->first + ".dds").c_str());
		InfernoEngine_Graphics::StoreTextureInMemory(textName.c_str());
		++iter;
	}
}

void AssetManager::LoadMaterials(){

	FileSystem fileSystem;
	MaterialLoader materialLoader;

	std::string materialFileExt = ".matList";

	std::vector<std::string> materials;

	std::string folderPath = fileSystem.GetMaterialsFolder();

	fileSystem.GetFilesInDirectory(folderPath, materialFileExt, materials);

	size_t fileCount = materials.size();
	for (size_t file = 0; file < fileCount; ++file){
		std::string filepath = folderPath + materials[file] + materialFileExt;
		materialLoader.LoadMaterials(m_umMaterials, filepath);
	}
}

void AssetManager::LoadAnimations()
{
	FileSystem fileSystem;
	FBXLoader loader;

	std::string fbxFileExt = ".fbx";
	std::string binFileExt = ".bin";

	std::vector<std::string> fbxAnimations;

	std::string folderPath = fileSystem.GetAnimationsFolder();

#if defined(SHIPPING)
	// Get all the file names from the animations folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, binFileExt, fbxAnimations);

	PRINT("Folder Name for Animations: " + folderPath + "\n");
	PRINT("Animations Found: " + std::to_string(fbxAnimations.size()) + "\n");
#else
	bool binFileExists = false;

	// Get all the file names from the animations folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, fbxFileExt, fbxAnimations);
#endif

	size_t fileCount = fbxAnimations.size();
	for (size_t file = 0; file < fileCount; file++)
	{
		Animation newAnimation;
		newAnimation.m_sName = fbxAnimations[file];

#if !defined(SHIPPING)
		// Does a binary file exist?
		binFileExists = fileSystem.FileExists(folderPath + fbxAnimations[file] + binFileExt);

		// If A binary file already exists AND it is newer than the fbx file..
		if (binFileExists == true && fileSystem.FileIsNewer(folderPath + fbxAnimations[file] + binFileExt, folderPath + fbxAnimations[file] + fbxFileExt) == true)
		{
#endif
			// Read animation data from the binary file
			loader.LoadAnimationFromBinary(folderPath + fbxAnimations[file] + binFileExt, newAnimation);
#if !defined(SHIPPING)
		}
		else
		{
			// Read animation data from the fbx file
			loader.LoadAnimationFromFBX(folderPath + fbxAnimations[file] + fbxFileExt, newAnimation);

			// Write animation data to a binary file
			loader.WriteAnimationToBinary(folderPath + fbxAnimations[file] + binFileExt, newAnimation);
		}
#endif

		// If the new animation is not already stored, store it in the animation map
		auto iter = m_umAnimations.find(newAnimation.m_sName);
		if (iter == m_umAnimations.end())
			m_umAnimations[newAnimation.m_sName] = newAnimation;
	}
}

void AssetManager::LoadAnimationSets()
{
	FileSystem fileSystem;

	std::string xmlFileExt = ".xml";

	std::vector<std::string> animationSets;

	std::string folderPath = fileSystem.GetAnimationSetsFolder();

	// Get all the file names from the animations folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, xmlFileExt, animationSets);

	// For each xml file..
	size_t fileCount = animationSets.size();
	for (size_t file = 0; file < fileCount; file++)
	{
		AnimationSet newSet;

		TiXmlDocument doc;
		doc.LoadFile((folderPath + animationSets[file] + ".xml").c_str());

		TiXmlElement* pRoot = doc.RootElement();

		if (pRoot == nullptr)
		{
			return;
		}

		TiXmlElement* pElement = pRoot->FirstChildElement("AnimSetData");

		// Get the name of this animation set
		const char* animSetName = pElement->GetText();
		newSet.m_sName = animSetName;

		// Get the animation count for this set
		int animCount = 0;
		pElement->Attribute("AnimCount", &animCount);

		// Reserve set vectors to match count
		newSet.ReserveSpace(animCount);

		// Get pointer to first set of Animation Data
		pElement = pElement->NextSiblingElement("AnimData");

		// Load in Animations and blend times for each animation transition
		for (int index1 = 0; index1 < animCount; index1++)
		{
			if (pElement == nullptr)
				break;

			Animation newAnim;

			// Get this animations name
			const char* animName = pElement->GetText();
			newAnim.m_sName = animName;

			// Load the animation from fbx or binary
			LoadAnimation(newAnim);

			for (int index2 = 0; index2 < animCount; index2++)
			{
				double tempTime;
				std::string attributeName = "Time" + std::to_string(index1) + std::to_string(index2);

				pElement->Attribute(attributeName.c_str(), &tempTime);

				newSet.AddBlendTime(index1, index2, (float)tempTime);
			}

			// Catch looping bool into int because xml is lame..
			int tempLooping;
			pElement->Attribute("Looping", &tempLooping);

			// If the temp int is greater or equal to one, the bool is true
			if (tempLooping >= 1)
				newAnim.SetLooping(true);
			else
				newAnim.SetLooping(false);

			// Store the animation in the set
			newSet.AddAnimation(newAnim);

			pElement = pElement->NextSiblingElement("AnimData");
		}

		// If the new animation set is not already stored, store it in the animation map
		auto iter = m_umAnimationSets.find(newSet.m_sName);
		if (iter == m_umAnimationSets.end())
			m_umAnimationSets[newSet.m_sName] = newSet;
	}
}

void AssetManager::LoadAnimationControllers()
{
	FileSystem fileSystem;

	std::string xmlFileExt = ".xml";

	std::vector<std::string> animationControllers;

	std::string folderPath = fileSystem.GetAnimationControllersFolder();

	// Get all the file names from the animations folder without their extensions
	fileSystem.GetFilesInDirectory(folderPath, xmlFileExt, animationControllers);

	// For each xml file..
	size_t fileCount = animationControllers.size();
	for (size_t file = 0; file < fileCount; file++)
	{
		AnimationController newController;

		TiXmlDocument doc;
		doc.LoadFile((folderPath + animationControllers[file] + ".xml").c_str());

		TiXmlElement* pRoot = doc.RootElement();

		if (pRoot == nullptr)
		{
			return;
		}

		TiXmlElement* pElement = pRoot->FirstChildElement("AnimControllerData");

		std::string name = pElement->Attribute("ControllerName");
		newController.m_sName = name;

		std::string animSet = pElement->Attribute("AnimationSet");
		newController.SetAnimationSet(GetAnimationSet(animSet));

		pElement = pElement->NextSiblingElement("Transition");

		while (pElement != nullptr)
		{
			// Get starting animation name
			std::string fromAnimName = pElement->Attribute("FromAnim");

			// Get data type
			int dataType;
			pElement->Attribute("DataType", &dataType);

			// Get event name
			std::string eventName = pElement->Attribute("EventName");

			// Get value in int form (cast later as needed)
			int value;
			pElement->Attribute("Value", &value);

			// Get operator used
			int oper;
			pElement->Attribute("Operator", &oper);

			// Get ending animation names (greater than 0 for similar animations)
			int animCount;
			pElement->Attribute("ToAnimCount", &animCount);

			std::vector<std::string> toAnimNames(animCount);

			for (size_t animIndex = 0; animIndex < toAnimNames.size(); animIndex++)
			{
				std::string animName = pElement->Attribute(("ToAnim" + std::to_string(animIndex)).c_str());
				toAnimNames[animIndex] = animName;
			}

			switch (dataType)
			{
			case 0: // bool
			{
				bool val;
				if (value > 0)
					val = true;
				else
					val = false;

				if (fromAnimName == "Any")
					newController.AddAnyStateTransition((AnimationController::DATA_TYPE)dataType, eventName, val, (AnimationController::OPERATOR)oper, toAnimNames);
				else
					newController.AddTransition(fromAnimName, (AnimationController::DATA_TYPE)dataType, eventName, val, (AnimationController::OPERATOR)oper, toAnimNames);
			}
			break;
			case 1: // int
			{
				if (fromAnimName == "Any")
					newController.AddAnyStateTransition((AnimationController::DATA_TYPE)dataType, eventName, value, (AnimationController::OPERATOR)oper, toAnimNames);
				else
					newController.AddTransition(fromAnimName, (AnimationController::DATA_TYPE)dataType, eventName, value, (AnimationController::OPERATOR)oper, toAnimNames);
			}
			break;
			case 2: // float
			{
				float val = (float)value;

				if (fromAnimName == "Any")
					newController.AddAnyStateTransition((AnimationController::DATA_TYPE)dataType, eventName, val, (AnimationController::OPERATOR)oper, toAnimNames);
				else
					newController.AddTransition(fromAnimName, (AnimationController::DATA_TYPE)dataType, eventName, val, (AnimationController::OPERATOR)oper, toAnimNames);
			}
			break;
			default:
				break;
			}

			pElement = pElement->NextSiblingElement("Transition");
		}

		// If the new animation is not already stored, store it in the animation map
		auto iter = m_umAnimationControllers.find(newController.m_sName);
		if (iter == m_umAnimationControllers.end())
			m_umAnimationControllers[newController.m_sName] = newController;
		//m_umAnimationControllers.insert(std::pair<std::string, AnimationController>(newController.m_sName, newController));
	}
}

void AssetManager::LoadAnimation(Animation& _animation)
{
	FileSystem fileSystem;
	FBXLoader loader;

	std::string fbxFileExt = ".fbx";
	std::string binFileExt = ".bin";

	std::string folderPath = fileSystem.GetAnimationsFolder();

#if !defined(SHIPPING)
	bool binFileExists = false;

	// Does a binary file exist?
	binFileExists = fileSystem.FileExists(folderPath + _animation.m_sName + binFileExt);

	// If A binary file already exists AND it is newer than the fbx file..
	if (binFileExists == true && fileSystem.FileIsNewer(folderPath + _animation.m_sName + binFileExt, folderPath + _animation.m_sName + fbxFileExt) == true)
	{
#endif
		// Read animation data from the binary file
		loader.LoadAnimationFromBinary(folderPath + _animation.m_sName + binFileExt, _animation);
#if !defined(SHIPPING)
	}
	else
	{
		// Read animation data from the fbx file
		loader.LoadAnimationFromFBX(folderPath + _animation.m_sName + fbxFileExt, _animation);

		// Write animation data to a binary file
		loader.WriteAnimationToBinary(folderPath + _animation.m_sName + binFileExt, _animation);
	}
#endif
}

void AssetManager::LoadPrefabs(){

	FileSystem fileSystem;
	PrefabLoader prefabLoader;

	std::string prefabExt = ".prefab";

	std::vector<std::string> prefabNames;

	std::string folderPath = fileSystem.GetPrefabsFolder();

	fileSystem.GetFilesInDirectory(folderPath, prefabExt, prefabNames);

	size_t fileCount = prefabNames.size();

	for (size_t file = 0; file < fileCount; ++file){
		Prefab* prefab = prefabLoader.LoadPrefab(folderPath + prefabNames[file] + prefabExt);

		if (prefab == nullptr) continue;

		InfernoEngine_ObjectManagement::CreatePrefab(prefab);

		delete prefab;
	}
}

// void AssetManager::ExportLevels()
// {
// 	FileSystem fileSystem;
// 	FBXLoader loader;
// 
// 	std::string fbxFileExt = ".fbx";
// 	std::string sceneFileExt = ".scene";
// 
// 	std::vector<std::string> fbxScenes;
// 
// 	std::string folderPath = fileSystem.GetScenesFolder();
// 
// 	bool sceneFileExists = false;
// 
// 	// Get all the file names from the animations folder without their extensions
// 	fileSystem.GetFilesInDirectory(folderPath, fbxFileExt, fbxScenes);
// 
// 	size_t fileCount = fbxScenes.size();
// 	for (size_t file = 0; file < fileCount; file++)
// 	{
// 		// Does a scene file exist?
// 		sceneFileExists = fileSystem.FileExists(folderPath + fbxScenes[file] + sceneFileExt);
// 
// 		// If A scene file already exists AND it is newer than the fbx file..
// 		if (sceneFileExists == true && fileSystem.FileIsNewer(folderPath + fbxScenes[file] + sceneFileExt, folderPath + fbxScenes[file] + fbxFileExt) == true)
// 		{
// 			// Skip this scene
// 			continue;
// 		}
// 		else
// 		{
// 			// Export level data to custom file format
// 			loader.LoadLevelFromFBX(folderPath + fbxScenes[file] + fbxFileExt);
// 		}
// 	}
// }