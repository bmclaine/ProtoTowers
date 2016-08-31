//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Asset Manager is responsible for handling the
//					loading of all assets.
//*********************************************************************//

#pragma once

#include <unordered_map>

#include "Assets\Mesh.h"
#include "Assets\Texture.h"
#include "Assets\AnimationController.h"
#include "PrefabLoader.h"
#include "GlobalIncludes.h"
#include "Assets\Material.h"

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	Mesh* GetMesh(std::string& _meshName);
	int GetTextureIndex(std::string& _textureName);
	Material* GetMaterial(std::string& _materialName);
	AnimationController* GetAnimationController(std::string& _controllerName);

	// TODO::DELETE
	Animation* GetAnimation(std::string& _animationName);
	// TODO::END

	//*********************************************************************//
	//	In:					Void
	//	Out:				Void
	//	Description:		Loads all assets into their proper containers
	//*********************************************************************//
	void LoadAssets();
	void LoadPrefabs();

private:
	void LoadMeshes();
	void LoadTextures();
	void LoadMaterials();
	void LoadAnimations();
	void LoadAnimationSets();
	void LoadAnimationControllers();
	void LoadAnimation(Animation& _animation);

	AnimationSet* GetAnimationSet(std::string& _animSetName);

	//void ExportLevels();

	std::unordered_map<std::string, Mesh> m_umMeshes;
	std::unordered_map<std::string, int> m_umTextures;
	std::unordered_map<std::string, Animation> m_umAnimations;
	std::unordered_map<std::string, AnimationSet> m_umAnimationSets;
	std::unordered_map<std::string, AnimationController> m_umAnimationControllers;
	std::unordered_map<std::string, Material> m_umMaterials;
};

