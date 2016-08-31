// Author: Jack Gillette / Zane Lewis
//
// Handles all GameObjects created through the engine and the allocation pools of prefabs. Takes care of updating / creation / destruction of 
// all GameObjects
#pragma once

#include "AllocationPool.h"
#include "InfernoTimer.h"
#include "ObjectRegistry.h"

#include <unordered_map>
#include <vector>

// === Forwards Declarations
class GameObject;
class Prefab;

// === Defines
#define INIT_MASTERLIST_SIZE 300
#define INIT_SCENELIST_SIZE 150
#define INIT_DELAYLIST_SIZE 50
#define INIT_ONENABLELIST_SIZE 20

class ObjectManager
{
private:
	// === Structures
	struct TimeDestroyGameObject {
		InfernoTimer m_Timer;
		GameObject * m_pGameObject;
	};

	// === Typedef
	typedef std::unordered_map<unsigned int, AllocationPool<GameObject>*>::iterator MapIter;

	// === Variables
	std::unordered_map<unsigned int, AllocationPool<GameObject>*> m_AllocatedPrefabs;
	std::vector<TimeDestroyGameObject*> m_vDelayDestroyedList;
	std::vector<GameObject*> m_vMasterObjectList;
	std::vector<GameObject*> m_vSceneObjects;
	std::vector<GameObject*> m_vPendingDestroy;
	std::vector<GameObject*> m_vPendingOnEnable;
	std::vector<GameObject*> m_vPendingOnDisable;
	ObjectRegistry* m_pComponentRegistry;
	bool m_bLockSceneList;
	bool m_bSceneSwitch;

	// === Private Interface === //
	void CheckForDeadObjects();
	void RemoveFromMasterList(GameObject * gameObject);
	void RemoveFromMasterList(size_t index);
	void RemoveFromSceneList(size_t index);
	// ========================= //

	// === Debug Interface === //
	void CheckIfObjectAlreadyInSceneList(GameObject* _gameObject);
	void CheckIfObjectIsNotInSceneList(GameObject* _gameObject);
	bool CheckIfObjectAlreadyInDestroyList(GameObject* _gameObject);
	void CheckIfAlreadyInPendingOnEnable(GameObject* _gameObject);
	// ======================= //

public:
	// === Cstr's / Dstr's === //
	ObjectManager();
	~ObjectManager();
	// ======================= //

	// === Interface === //
	void Initialize();
	void Shutdown();

	void Update();
	void FixedUpdate();
	void LateUpdate();
	void OnDrawGizmos();

	// === Instantiate 
	//  IN : string - Prefab name
	//  OUT : GameObject* - The created GameObject
	//
	//  Instantiates a GameObject of the given prefab name, if that prefab exists. If the name is left blank, returns just a new blank GameObject. Handles
	//  properly registering the GameObjects with MasterList / SceneList
	GameObject* Instantiate(std::string _prefabName = std::string());
	void Destroy(GameObject* _gameObject, float _delayTime = 0.0f);
	void TrueDestroy(GameObject* _gameObject);
	void RemoveAllGameObjects();
	void RemoveFromSceneList(GameObject * gameObject);

	bool AllocatePrefab(std::string _prefabName, GameObject* _prefabObject, unsigned int _amount);
	bool CreatePrefab(Prefab* _prefabData, unsigned int _allocationAmount = 1);
	
	void RegisterObject(GameObject* _gameObject);
	GameObject* GetGameObjectByName(std::string& _gameObjectName);
	void AddForOnEnable(GameObject* _gameObject);
	void RemoveFromOnEnable(GameObject* _gameObject);
	void AddForOnDisable(GameObject* _gameObject);

	IComponent* CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform);
	// ================= //

	// === Templated Interface === //
	template <typename type>
	unsigned int RegisterComponent(const char* _componentID);
	template <typename type>
	unsigned int RegisterUIComponent(const char* _uiComponentID);
	// =========================== //
};

// === Templated Interface === //
template <typename type>
unsigned int ObjectManager::RegisterComponent(const char* _componentID)
{
	return m_pComponentRegistry->RegisterComponentType<type>(_componentID);
}

template <typename type>
unsigned int ObjectManager::RegisterUIComponent(const char* _uiComponentID)
{
	return m_pComponentRegistry->RegisterUIComponentType<type>(_uiComponentID);
}
// =========================== //
