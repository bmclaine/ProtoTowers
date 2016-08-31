// Author: Jonathan Gyurkovics
//
// The GameObject that will be used for any active object in the game

#pragma once

#include "Object.h"

#include <vector>

#include "MathLib.h"
#include "Flags.h"
#include "Components\Transform.h"

#include<stdint.h>

class IComponent;
class Collider;

enum class CollisionLayers {
	LAYER_DEFAULT = 0,
	LAYER_FRIENDLY = 1,
	LAYER_ENEMY = 2,
	LAYER_FRIENDLY_PROJ = 3,
	LAYER_ENEMY_PROJ = 4,
	LAYER_STATIC_WORLD = 5,
	LAYER_BARRIER = 6,
	LAYER_CAMERA = 7,
	LAYER_PLAYER_SHIELD = 8,
	LAYER_COUNT
};

class GameObject : public Object
{
private:
	enum class GAMEOBJECT_FLAGS : uint8_t {
		ACTIVE = 0,
		POSTINIT_CALLED = 2,
		ADDEDFORONENABLE = 3,
		IsStatic = 4,
		TOTAL_FLAGS = 5,
	};

	std::vector<IComponent*> m_vComponents;		// Holds the list of components the current game object has

	std::string m_sName;						// Holds the name of the Game Object
	std::string m_sTag;							// Holds the tag of the Game Object
	unsigned int m_uiLayer;						// Holds the layer the Game Object is on
	unsigned int m_uiPrefabID = 0;				// Holds the Prefab ID of the object, if there is one
	Flags<char> m_fcFlags;						// Holds the flags for the Game Object

public:
	GameObject();
	GameObject(const Vector3& Position, const Quaternion& Rotation = Quaternion::Identity(), const Vector3& Scale = { 1.0f, 1.0f, 1.0f });
	GameObject(const Vector3& Position, const Vector3& Rotation, const Vector3& Scale = { 1.0f, 1.0f, 1.0f });
	GameObject(Transform& _transform);
	GameObject(const GameObject& _copy);
	~GameObject();

	GameObject& operator=(const GameObject& _rhs);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	void OnEnable();
	void OnDisable();

	// IN: void
	// OUT: void
	//
	// Updates all of the game object's components
	virtual void Update();

	// IN: void
	// OUT: void
	//
	// Updates all of the game object's components
	virtual void FixedUpdate();

	// IN: void
	// OUT: void
	//
	// Updates all of the game object's components
	virtual void LateUpdate();

	// IN: void
	// OUT: void
	//
	// Calls the components OnDrawGizmos function
	virtual void OnDrawGizmos();

	// IN: Collider - pointer to the collider that collided with this object
	// OUT: void
	//
	// Called on the first frame that collision has been detected
	virtual void OnCollisionEnter(Collider*);
	// IN: Collider - pointer to the collider to the collider that is still colliding with this object
	// OUT: void
	//
	// Called on each sequential frame that collision is detected
	virtual void OnCollisionStay(Collider*);
	// IN: Collider - pointer to collider that is no longer colliding with this object
	// OUT: void
	//
	// Called on first frame that collision is no longer detected
	virtual void OnCollisionExit(Collider*);

	// IN: Collider - pointer to collider that entered the trigger
	// OUT: void
	//
	// Called on the first frame that collision with a trigger is detected
	virtual void OnTriggerEnter(Collider*);
	// IN: Collider - pointer to collider that still inside of the trigger
	// OUT: void
	//
	// Called each sequential frame after collision with a trigger is detected
	virtual void OnTriggerStay(Collider*);
	// IN: Collider - pointer to collider that exited trigger
	// OUT: void
	//
	// Called when collision is no longer detected with a trigger
	virtual void OnTriggerExit(Collider*);

	void OnDestroy();

	// IN: string - String to compare
	// OUT: bool - true if the tags match
	//
	// Compares the tag to the incoming string
	bool CompareTag(const char* tag) const;
	// IN: string - String to compare
	// OUT: bool - true if the tags match
	//
	// Compares the tag to the incoming string
	bool CompareTag(const std::string tag) const;

	// IN: void
	// OUT: bool - true if all parents are active as well as this
	//
	// Returns if the object is considered active in the hierarchy
	bool ActiveInHierarchy() const;
	// IN: void
	// OUT: bool - true if object is active
	//
	// Returns if the object is active
	bool IsActive() const;
	// IN: void
	// OUT: unsigned int - Physics layer the object is on
	//
	// Returns the objects layer
	unsigned int GetLayer() const;
	// IN: void
	// OUT: string - name of the object
	//
	// Returns the objects name
	std::string GetName() const;
	// IN: void
	// OUT: string - tag of the object
	//
	// Returns the objects tag
	std::string GetTag() const;

	unsigned int GetPrefabID() const;

	bool IsStatic() const;

	// IN: bool - Whether or not to make the GameObject Active
	//     bool - Whether or not to have the call happen instantly, or next frame
	// OUT: void
	//
	// Sets the GameObject to active or not, with the choice of having it happen next frame, only for Disabling the gameobject. Enabling the GameObject
	// always happens next frame.
	void SetActive(bool _active, bool _nextFrame = false);
	void SetLayer(unsigned int layer);
	void SetName(std::string name);
	void SetName(const char* name);
	void SetTag(std::string tag);
	void SetTag(const char* tag);
	void SetPrefabID(unsigned int _id);
	void SetIsStatic(bool isStatic);

	// Component based calls

	// IN: void
	// OUT: void
	//
	// Adds a default version of the passed in component
	template<typename T>
	T* AddComponent();

	// IN: T& - Component to add and copy
	// OUT: void
	//
	// Adds a copy of the passed in component
	template<typename T>
	void AddComponent(T& component);

	// IN: string - name of the component to add
	// OUT: void
	//
	// Adds a default version of the component based on incoming string
	IComponent* AddComponent(std::string& component);

	// For now be careful on calling GetComponent calls. They are currently using a dynamic_cast check
	// to enable the ability to grab like components of what was asked for. This allows for the use of
	// calling for something like Collider and getting back all types of Colliders instead of having to
	// be specific on what you are looking for.

	// IN: bool - grabs disabled components if true
	// OUT: T* - pointer to the component found
	//
	// Returns the first instance of the component found
	template<typename T>
	T* GetComponent(bool grabDisabled = true) const;

	// IN: bool - grabs disabled components if true
	// OUT: std::vector<T*> - vector of components found
	//
	// Returns all found instances of the component found
	template<typename T>
	std::vector<T*> GetComponents(bool grabDisabled = true) const;

	// IN: bool - grabs disabled components if true
	// OUT: T* - pointer to the component found
	//
	// Returns the first instance of the component found in the children
	template<typename T>
	T* GetComponentInChildren(bool grabDisabled = true) const;

	// IN: bool - grabs disabled components if true
	// OUT: std::vector<T*> - vector of components found
	//
	// Returns all found instances of the component found in the children
	template<typename T>
	std::vector<T*> GetComponentsInChildren(bool grabDisabled = true) const;

	// IN: bool - grabs disabled components if true
	// OUT: T* - pointer to the component found
	//
	// Returns the first instance of the component found in the parent
	template<typename T>
	T* GetComponentInParent(bool grabDisabled = true) const;

	// IN: bool - grabs disabled components if true
	// OUT: std::vector<T*> - vector of components found
	//
	// Returns all found instances of the component found in the parent
	template<typename T>
	std::vector<T*> GetComponentsInParent(bool grabDisabled = false) const;

	Transform* const transform;
};

// IN: void
// OUT: void
//
// Adds a default version of the passed in component
template<typename T>
T* GameObject::AddComponent()
{
	// Calls the default constructor passing in the game object as the owner
	T* newComponent = new T(this, transform);

	m_vComponents.push_back(newComponent);
	newComponent->m_fcFlags.SetFlag((unsigned int)COMPONENT_FLAGS::IS_ENABLED, (unsigned int)true);

	if (m_fcFlags.GetFlag((unsigned int)GAMEOBJECT_FLAGS::POSTINIT_CALLED)) {
		newComponent->Init();
	}

	return newComponent;
}

// IN: T& - Component to add and copy
// OUT: void
//
// Adds a copy of the passed in component
template<typename T>
void GameObject::AddComponent(T& component)
{
	// Creates a new component and calls the assignment operator to copy over all non-const values
	T* newComponent = component.CreateCopy(this, transform);

	if (m_fcFlags.GetFlag((unsigned int)GAMEOBJECT_FLAGS::POSTINIT_CALLED)) {
		newComponent->Init();
	}

	m_vComponents.push_back(newComponent);
}

// IN: bool - grabs disabled components if true
// OUT: T* - pointer to the component found
//
// Returns the first instance of the component found
template<typename T>
T* GameObject::GetComponent(bool grabDisabled) const
{
	T* transformedComponent = nullptr;

	// Look through all the components the game object has to see if we can find the one
	// that we are looking for
	for (size_t comp = 0; comp < m_vComponents.size(); comp++)
	{
		if (grabDisabled || m_vComponents[comp]->isEnabled())
		{
			transformedComponent = dynamic_cast<T*>(m_vComponents[comp]);

			if (transformedComponent != nullptr)
				return transformedComponent;
		}
	}

	return nullptr;
}

// IN: bool - grabs disabled components if true
// OUT: std::vector<T*> - vector of components found
//
// Returns all found instances of the component found
template<typename T>
std::vector<T*> GameObject::GetComponents(bool grabDisabled) const
{
	// Create a vector to store all found components
	std::vector<T*> components;
	T* transformedComponent = nullptr;

	// Check each component to see if they are what we are looking for
	for (size_t comp = 0; comp < m_vComponents.size(); comp++)
	{
		if (grabDisabled || m_vComponents[comp]->isEnabled())
		{
			transformedComponent = dynamic_cast<T*>(m_vComponents[comp]);

			if (transformedComponent != nullptr)
				components.push_back(transformedComponent);
		}
	}

	return components;
}

// IN: bool - grabs disabled components if true
// OUT: T* - pointer to the component found
//
// Returns the first instance of the component found in the children
template<typename T>
T* GameObject::GetComponentInChildren(bool grabDisabled) const
{
	T* transformedComponent = nullptr;

	// Check each child to see if they have the component we are looking for
	for (size_t child = 0; child < transform->m_vChildren.size(); child++)
	{
		transformedComponent = transform->m_vChildren[child]->gameObject->GetComponent<T>(grabDisabled);

		if (transformedComponent != nullptr)
			return transformedComponent;
	}

	// If we didn't find the component in our children, check their children
	for (size_t child = 0; child < transform->m_vChildren.size(); child++)
	{
		transformedComponent = transform->m_vChildren[child]->gameObject->GetComponentInChildren<T>(grabDisabled);

		if (transformedComponent != nullptr)
			return transformedComponent;
	}

	// If the component was not found at all
	return nullptr;
}

// IN: bool - grabs disabled components if true
// OUT: std::vector<T*> - vector of components found
//
// Returns all found instances of the component found in the children
template<typename T>
std::vector<T*> GameObject::GetComponentsInChildren(bool grabDisabled) const
{
	// Create a vector to store all found components
	std::vector<T*> components;
	T* transformedComponent = nullptr;

	// Check each child to see if they have the component we are looking for
	for (size_t child = 0; child < transform->m_vChildren.size(); child++)
	{
		// Search the child for the component type we are looking for
		std::vector<T*> foundComponents = transform->m_vChildren[child]->GetComponents<T>(grabDisabled);

		// For each component found in the child, add to the components vector
		for (size_t comp = 0; comp < foundComponents.size(); comp++)
			components.push_back(foundComponents[comp]);

		// Search the child's children for the component
		foundComponents = transform->m_vChildren[child]->GetComponentsInChildren<T>(grabDisabled);

		// For each component found in the child's children, add to the components vector
		for (size_t comp = 0; comp < foundComponents.size(); comp++)
			components.push_back(foundComponents[comp]);
	}

	return components;
}

// IN: bool - grabs disabled components if true
// OUT: T* - pointer to the component found
//
// Returns the first instance of the component found in the parent
template<typename T>
T* GameObject::GetComponentInParent(bool grabDisabled) const
{
	if (transform->m_pParent)
		return transform->m_pParent->gameObject->GetComponent<T>(grabDisabled);

	return nullptr;
}

// IN: bool - grabs disabled components if true
// OUT: std::vector<T*> - vector of components found
//
// Returns all found instances of the component found in the parent
template<typename T>
std::vector<T*> GameObject::GetComponentsInParent(bool grabDisabled) const
{
	if (transform->m_pParent)
		return transform->m_pParent->gameObject->GetComponents<T>(grabDisabled);

	return nullptr;
}