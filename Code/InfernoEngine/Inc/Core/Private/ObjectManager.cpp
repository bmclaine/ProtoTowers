#include "ObjectManager.h"
#include "GameObject.h"
#include "AllocationPool.h"
#include "InfernoTime.h"
#include "InfernoTimer.h"
#include "AudioManager.h"
#include "PrefabLoader.h"

// === Engine Components
#include "Components\Animator.h"
#include "Components\Camera.h"
#include "Components\Controller.h"
#include "Components\Light.h"
#include "Components\MeshRenderer.h"
#include "Components\NavAgent.h"
#include "Components\ParticleSystem.h"
#include "Components\SkinnedMeshRenderer.h"
#include "Components\Transform.h"
#include "Components\SpriteRenderer.h"
#include "Components\AudioListener.h"
#include "Physics\BoxCollider.h"
#include "Physics\SphereCollider.h"
#include "Physics\CapsuleCollider.h"
#include "Physics\Rigidbody.h"

#include "UIComponents\RectTransform.h"
#include "UIComponents\UIButton.h"
#include "UIComponents\UICanvas.h"
#include "UIComponents\UIEventSystem.h"
#include "UIComponents\UIImage.h"
#include "UIComponents\UIText.h"

// ===== Constructors / Destructors ===== //
ObjectManager::ObjectManager()
{
	m_bLockSceneList = false;
	m_bSceneSwitch = false;
	m_pComponentRegistry = new ObjectRegistry();
}

ObjectManager::~ObjectManager()
{
	// === Delete any Delay Destroyed Objects
	for (int i = 0; i < m_vDelayDestroyedList.size(); ++i) {
		delete m_vDelayDestroyedList[i];
	}
	m_vDelayDestroyedList.clear();

	// === Delete all Created Objects
	for (int i = 0; i < m_vMasterObjectList.size(); ++i) {
		if (m_vMasterObjectList[i] != nullptr) {
			// == Only Delete the Object, not it's children
			m_vMasterObjectList[i]->transform->LoseChildren();
			delete m_vMasterObjectList[i];
		}
	}

	// === Delete the AllocatedPools
	MapIter iter = m_AllocatedPrefabs.begin();
	while (iter != m_AllocatedPrefabs.end()) {
		delete iter->second;
		++iter;
	}

	delete m_pComponentRegistry;
}
// ====================================== //

// ===== Interface ===== //
void ObjectManager::Initialize()
{
	m_vMasterObjectList.reserve(INIT_MASTERLIST_SIZE);
	m_vSceneObjects.reserve(INIT_SCENELIST_SIZE);
	m_vDelayDestroyedList.reserve(INIT_DELAYLIST_SIZE);
	m_vPendingOnEnable.reserve(INIT_ONENABLELIST_SIZE);
	m_vPendingOnDisable.reserve(INIT_ONENABLELIST_SIZE);

	// === Register all Engine-Side Components / UIComponents
	m_pComponentRegistry->RegisterComponentType<Animator>("Animator");
	m_pComponentRegistry->RegisterComponentType<Camera>("Camera");
	m_pComponentRegistry->RegisterComponentType<Light>("Light");
	m_pComponentRegistry->RegisterComponentType<MeshRenderer>("MeshRenderer");
	m_pComponentRegistry->RegisterComponentType<NavAgent>("NavAgent");
	m_pComponentRegistry->RegisterComponentType<ParticleSystem>("ParticleSystem");
	m_pComponentRegistry->RegisterComponentType<SkinnedMeshRenderer>("SkinnedMeshRenderer");
	m_pComponentRegistry->RegisterComponentType<SpriteRenderer>("SpriteRenderer");
	m_pComponentRegistry->RegisterComponentType<Transform>("Transform");
	m_pComponentRegistry->RegisterComponentType<BoxCollider>("BoxCollider");
	m_pComponentRegistry->RegisterComponentType<SphereCollider>("SphereCollider");
	m_pComponentRegistry->RegisterComponentType<CapsuleCollider>("CapsuleCollider");
	m_pComponentRegistry->RegisterComponentType<Rigidbody>("Rigidbody");
	m_pComponentRegistry->RegisterComponentType<AudioListener>("AudioListener");

	m_pComponentRegistry->RegisterUIComponentType<RectTransform>("RectTransform");
	m_pComponentRegistry->RegisterUIComponentType<UIButton>("UIButton");
	m_pComponentRegistry->RegisterUIComponentType<UICanvas>("UICanvas");
	m_pComponentRegistry->RegisterUIComponentType<UIEventSystem>("UIEventSystem");
	m_pComponentRegistry->RegisterUIComponentType<UIImage>("UIImage");
	m_pComponentRegistry->RegisterUIComponentType<UIText>("UIText");
}

void ObjectManager::Shutdown()
{

}

void ObjectManager::Update()
{
	size_t count;
	size_t i;

	// === Destroy any GameObjects pending to be destroyed from last frame
	for (i = 0; i < m_vPendingDestroy.size(); ++i) {
		TrueDestroy(m_vPendingDestroy[i]);
	}
	m_vPendingDestroy.clear();

	// === Check for DeadObjects in the TimedDestroy list
	CheckForDeadObjects();

	if (m_bSceneSwitch) {
		m_bSceneSwitch = false;
		return;
	}

	// === Call OnEnable on any newly added GameObjects
	for (i = 0; i < m_vPendingOnEnable.size(); ++i) {
		m_vPendingOnEnable[i]->OnEnable();
	}
	m_vPendingOnEnable.clear();

	// === Call OnDisable on any newly added GameObjects
	for (i = 0; i < m_vPendingOnDisable.size(); ++i) {
		m_vPendingOnDisable[i]->OnDisable();
	}
	m_vPendingOnDisable.clear();

	// === Update all Active Objects in scene
	count = m_vSceneObjects.size();
	for (i = 0; i < m_vSceneObjects.size(); i++) {
		if (m_vSceneObjects[i]->ActiveInHierarchy()) {
			m_vSceneObjects[i]->Update();
		}
	}
}

void ObjectManager::FixedUpdate()
{
	// === Update all Objects in scene
	for (size_t i = 0; i < m_vSceneObjects.size(); i++) {
		if (m_vSceneObjects[i]->ActiveInHierarchy()) {
			m_vSceneObjects[i]->FixedUpdate();
		}
	}
}

void ObjectManager::LateUpdate()
{
	// === Update all Objects in scene
	for (size_t i = 0; i < m_vSceneObjects.size(); i++) {
		if (m_vSceneObjects[i]->ActiveInHierarchy()) {
			m_vSceneObjects[i]->LateUpdate();
		}
	}
}

void ObjectManager::OnDrawGizmos()
{
	for (size_t i = 0; i < m_vSceneObjects.size(); i++) {
		m_vSceneObjects[i]->OnDrawGizmos();
	}
}

// === Instantiate 
//  IN : string - Prefab name
//  OUT : GameObject* - The created GameObject
//
//  Instantiates a GameObject of the given prefab name, if that prefab exists. If the name is left blank, returns just a new blank GameObject. Handles
//  properly registering the GameObjects with MasterList / SceneList
GameObject* ObjectManager::Instantiate(std::string _prefabName)
{
	if (_prefabName == std::string()) {
		GameObject* newObject = new GameObject();
		//newObject->Init();
		return newObject;
	}

	unsigned int prefabID = (unsigned int)std::hash<std::string>()(_prefabName);

	MapIter iter = m_AllocatedPrefabs.find(prefabID);
	if (iter == m_AllocatedPrefabs.end()) {
		GameObject* _gameObject = new GameObject();
		m_vSceneObjects.push_back(_gameObject);
		_gameObject->SetActive(true);
		_gameObject->Init();
		return _gameObject;
	}

	m_bLockSceneList = true;
	GameObject * _gameObject = iter->second->Allocate();
	_gameObject->Init();
	m_bLockSceneList = false;

	m_vSceneObjects.push_back(_gameObject);
	_gameObject->SetActive(true);

	return _gameObject;
}

void ObjectManager::Destroy(GameObject* _gameObject, float _delayTime)
{
	if (_delayTime > 0.0f) {
		TimeDestroyGameObject* obj = new TimeDestroyGameObject;
		obj->m_pGameObject = _gameObject;
		obj->m_Timer = InfernoTimer(_delayTime);
		m_vDelayDestroyedList.push_back(obj);
		return;
	}

	// === Make Sure it isn't already Marked to Destroy
	if (CheckIfObjectAlreadyInDestroyList(_gameObject)) {
		return;
	}

	m_vPendingDestroy.push_back(_gameObject);

	// === Is the Object a prefab?
	/*if (_gameObject->GetPrefabID() != 0) {
		_gameObject->transform->SetParent(nullptr);
	}*/
}

void ObjectManager::TrueDestroy(GameObject* _gameObject)
{
	if (_gameObject->GetPrefabID() == 0) {
		// === Just remove from scene list and master list
		RemoveFromSceneList(_gameObject);

		GameObject* toDelete = nullptr;

		size_t count = m_vMasterObjectList.size();
		for (size_t i = 0; i < count; ++i) {
			if (m_vMasterObjectList[i] == _gameObject) {
				// == Remove from master list
				toDelete = m_vMasterObjectList[i];
				RemoveFromMasterList(i);
				// == Unregister from the AudioManager
				AudioManager::Instance()->UnRegisterListener(toDelete);
				_gameObject->SetActive(false);
				_gameObject->OnDestroy();
				_gameObject->transform->SetParent(nullptr);
				// == Delete
				delete toDelete;
				break;
			}
		}
	}
	else {
		// === Remove from Scene list and deallocate
		RemoveFromSceneList(_gameObject);

		_gameObject->OnDestroy();

		_gameObject->transform->SetParent(nullptr);

		m_AllocatedPrefabs[_gameObject->GetPrefabID()]->Deallocate(_gameObject);

		AudioManager::Instance()->UnRegisterListener(_gameObject);
	}
}

void ObjectManager::RemoveAllGameObjects()
{
	for (size_t i = 0; i < m_vSceneObjects.size(); i++) {
		Destroy(m_vSceneObjects[i]);
	}
	m_vSceneObjects.clear();

	size_t count = m_vDelayDestroyedList.size();
	for (size_t i = 0; i < count; ++i) {
		delete m_vDelayDestroyedList[i];
	}

	m_vDelayDestroyedList.clear();

	m_bSceneSwitch = true;
}


bool ObjectManager::AllocatePrefab(std::string _prefabName, GameObject* _prefabObject, unsigned int _amount)
{
	unsigned int prefabID = (unsigned int)std::hash<std::string>()(_prefabName);

	MapIter iter = m_AllocatedPrefabs.find(prefabID);
	if (iter != m_AllocatedPrefabs.end()) {
		// === A prefab with this name already exists
		return false;
	}

	// === Set the Settings for this Object to be the base prefab
	_prefabObject->SetActive(false);
	_prefabObject->SetPrefabID(prefabID);
	RemoveFromSceneList(_prefabObject);
	RemoveFromOnEnable(_prefabObject);

	if (_prefabName == "TurretProjectile") {
		int debug = 0;
		_prefabObject->SetTag("Deactivated");
	}

	m_bLockSceneList = true;
	m_AllocatedPrefabs.insert(std::make_pair(prefabID, new AllocationPool<GameObject>(_prefabObject, _amount, false)));
	m_bLockSceneList = false;

	return true;
}

bool ObjectManager::CreatePrefab(Prefab* _prefabData, unsigned int allocationAmount)
{
	std::map<unsigned int, Object*> objectMap;
	std::map<unsigned int, ObjectData*> dataMap;
	std::vector<Transform*> childTransforms;
	GameObject* root = nullptr;
	std::string prefabName;

	// === Lock the Scene List
	m_bLockSceneList = true;

	unsigned int objectDataCount = _prefabData->objects.size();

	for (unsigned int i = 0; i < objectDataCount; ++i){
		if (_prefabData->objects[i]->name != "GameObject") continue;

		ObjectData* curObjectData = _prefabData->objects[i];
		Property* guidProperty = curObjectData->GetProperty(std::string("GUID"));
		unsigned int guidValue = (unsigned int)atoi(guidProperty->value.c_str());
		dataMap.emplace(guidValue, curObjectData);

		GameObject* newGameObject = new GameObject();
		newGameObject->SetInstanceID(guidValue);
		objectMap.emplace(guidValue, newGameObject);
	}

	for (unsigned int i = 0; i < objectDataCount; ++i){
		if (_prefabData->objects[i]->name != "Transform") continue;

		ObjectData* curObjectData = _prefabData->objects[i];
		Property* guidProperty = curObjectData->GetProperty(std::string("GUID"));
		unsigned int guidValue = (unsigned int)atoi(guidProperty->value.c_str());
		dataMap.emplace(guidValue, curObjectData);

		Property* gameObjectProperty = curObjectData->GetProperty(std::string("gameObject"));
		unsigned int gameObjectGuid = (unsigned int)atoi(gameObjectProperty->value.c_str());
		GameObject* referencedGameObject = static_cast<GameObject*>(objectMap.find(gameObjectGuid)->second);

		referencedGameObject->transform->SetInstanceID(guidValue);
		objectMap.emplace(guidValue, referencedGameObject->transform);

		Property* parentProperty = curObjectData->GetProperty(std::string("Parent"));
		if (parentProperty == nullptr){
			root = referencedGameObject;
			ObjectData* gameObjectData = dataMap.find(gameObjectGuid)->second;
			Property* prefabNameProp = gameObjectData->GetProperty(std::string("PrefabName"));
			prefabName = prefabNameProp->value;
		}
		else{
			childTransforms.push_back(referencedGameObject->transform);
		}
	}

	for (unsigned int i = 0; i < objectDataCount; ++i){
		if (_prefabData->objects[i]->name == "GameObject" || _prefabData->objects[i]->name == "Transform") continue;

		ObjectData* curObjectData = _prefabData->objects[i];
		Property* guidProperty = curObjectData->GetProperty(std::string("GUID"));
		unsigned int guidValue = (unsigned int)atoi(guidProperty->value.c_str());
		dataMap.emplace(guidValue, curObjectData);

		Property* gameObjectProperty = curObjectData->GetProperty(std::string("gameObject"));
		unsigned int gameObjectGuid = (unsigned int)atoi(gameObjectProperty->value.c_str());
		GameObject* referencedGameObject = static_cast<GameObject*>(objectMap.find(gameObjectGuid)->second);

		IComponent* component = referencedGameObject->AddComponent(curObjectData->name);
		component->SetInstanceID(guidValue);
		objectMap.emplace(guidValue, component);
	}

	auto objectIter = objectMap.begin();

	for (unsigned int i = 0; i < childTransforms.size(); ++i){
		Transform* childTransform = childTransforms[i];
		ObjectData* data = dataMap.find(childTransform->GetInstanceID())->second;
		Property* parentProp = data->GetProperty(std::string("Parent"));
		unsigned int parentGUID = (unsigned int)atoi(parentProp->value.c_str());
		Transform* parent = (Transform*)objectMap.find(parentGUID)->second;
		childTransform->SetParent(parent);
	}

	for (; objectIter != objectMap.end(); ++objectIter){
		Object* obj = objectIter->second;
		obj->PostInit(objectMap, dataMap);
	}

	if (prefabName == ""){
		int debug = 0;
	}

	// === Unlock the Scene List
	m_bLockSceneList = false;

	if (root)
		return AllocatePrefab(prefabName, root, _prefabData->allocationAmount);

	return false;
}

IComponent* ObjectManager::CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform)
{
	return m_pComponentRegistry->CreateComponent(_id, _gameObject, _transform);
}


void ObjectManager::RegisterObject(GameObject* _gameObject)
{
	m_vMasterObjectList.push_back(_gameObject);
	if (!m_bLockSceneList) {
#if (defined(DEBUG) || defined(_DEBUG)) 
		// === DEBUG 
		CheckIfObjectAlreadyInSceneList(_gameObject);
		CheckIfAlreadyInPendingOnEnable(_gameObject);
		// ===
#endif

		m_vSceneObjects.push_back(_gameObject);
		m_vPendingOnEnable.push_back(_gameObject);
	}
}

GameObject* ObjectManager::GetGameObjectByName(std::string& _gameObjectName)
{
	size_t count = m_vSceneObjects.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vSceneObjects[i]->GetName() == _gameObjectName) {
			return m_vSceneObjects[i];
		}
	}

	GameObject* foundObject = nullptr;
	for (size_t i = 0; i < count; ++i) {
		foundObject = m_vSceneObjects[i]->transform->GetChildByName(_gameObjectName);
		if (foundObject != nullptr) {
			return foundObject;
		}
	}

	return nullptr;
}

void ObjectManager::AddForOnEnable(GameObject* _gameObject)
{
#if (defined(DEBUG) || defined(_DEBUG)) 
	// === DEBUG 
	CheckIfAlreadyInPendingOnEnable(_gameObject);
	// ===
#endif

	m_vPendingOnEnable.push_back(_gameObject);
}

void ObjectManager::RemoveFromOnEnable(GameObject* _gameObject)
{
	size_t count = m_vPendingOnEnable.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vPendingOnEnable[i] == _gameObject) {
			if (count > 1) {
				std::iter_swap(m_vPendingOnEnable.begin() + i, --m_vPendingOnEnable.end());
			}
			m_vPendingOnEnable.erase(m_vPendingOnEnable.end() - 1);
			break;
		}
	}
}

void ObjectManager::AddForOnDisable(GameObject* _gameObject)
{
	m_vPendingOnDisable.push_back(_gameObject);
}
// ===================== //

// ===== Private Interface ===== //
void ObjectManager::CheckForDeadObjects()
{
	size_t count = m_vDelayDestroyedList.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vDelayDestroyedList[i]->m_Timer.IsFinished()) {
			Destroy(m_vDelayDestroyedList[i]->m_pGameObject, 0.0f);
			delete m_vDelayDestroyedList[i];

			m_vDelayDestroyedList[i] = m_vDelayDestroyedList[count - 1];
			m_vDelayDestroyedList.erase(--m_vDelayDestroyedList.end());
			--i; --count;
		}
	}
}

void ObjectManager::RemoveFromMasterList(GameObject * gameObject)
{
	size_t count = m_vMasterObjectList.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vMasterObjectList[i] == gameObject) {
			// === Make sure this isn't the only GameObject in the list
			if (m_vMasterObjectList.size() > 1) {
				std::iter_swap(m_vMasterObjectList.begin() + i, --m_vMasterObjectList.end());
			}
			m_vMasterObjectList.erase(--m_vMasterObjectList.end());
			break;
		}
	}
}

void ObjectManager::RemoveFromSceneList(GameObject * gameObject)
{
	size_t count = m_vSceneObjects.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vSceneObjects[i] == gameObject) {
			// === Make sure this isn't the only GameObject in the list
			if (count > 1) {
				std::iter_swap(m_vSceneObjects.begin() + i, --m_vSceneObjects.end());
			}
			m_vSceneObjects.erase(--m_vSceneObjects.end());
			return;
		}
	}
}
void ObjectManager::RemoveFromMasterList(size_t index)
{
	// === Make sure this isn't the only GameObject in the list
	if (m_vMasterObjectList.size() > 1) {
		std::iter_swap(m_vMasterObjectList.begin() + index, --m_vMasterObjectList.end());
	}

	m_vMasterObjectList.erase(--m_vMasterObjectList.end());
}

void ObjectManager::RemoveFromSceneList(size_t index)
{
	// === Make sure this isn't the only GameObject in the list
	if (m_vSceneObjects.size() > 1) {
		std::iter_swap(m_vSceneObjects.begin() + index, --m_vSceneObjects.end());
	}

	m_vSceneObjects.erase(--m_vSceneObjects.end());
}
// ============================= //

// ===== Debug Interface ===== //
void ObjectManager::CheckIfObjectAlreadyInSceneList(GameObject* _gameObject)
{
	size_t count = m_vSceneObjects.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vSceneObjects[i] == _gameObject) {
			DebugBreak();
			return;
		}
	}
}

void ObjectManager::CheckIfObjectIsNotInSceneList(GameObject* _gameObject)
{
	// === If the GameObject has a Parent, it won't be in the SceneList
	if (_gameObject->transform->GetParent() != nullptr || _gameObject->GetName() != "Mini_Minion_Red") {
		return;
	}

	// === Search through the SceneList
	size_t count = m_vSceneObjects.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vSceneObjects[i] == _gameObject) {
			return;
		}
	}
	// === Debug Point
	int debug = 0;
}

bool ObjectManager::CheckIfObjectAlreadyInDestroyList(GameObject* _gameObject)
{
	size_t count = m_vPendingDestroy.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vPendingDestroy[i] == _gameObject) {
			return true;
		}
	}

	return false;
}

void ObjectManager::CheckIfAlreadyInPendingOnEnable(GameObject* _gameObject)
{
	size_t count = m_vPendingOnEnable.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vPendingOnEnable[i] == _gameObject) {
			DebugBreak();
			return;
		}
	}
}
// =========================== //