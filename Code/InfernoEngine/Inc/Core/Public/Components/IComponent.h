// Author: Jonathan Gyurkovics
//
// The interface class in which all components will derive off of

#pragma once

#include "../Object.h"

#include "Flags.h"

class Transform;
class GameObject;
class Collider;

enum COMPONENT_FLAGS {
	IS_ENABLED,
	POST_INIT_CALLED,
	TOTAL_FLAGS,
};

class IComponent : public Object
{
protected:
	Flags<char> m_fcFlags;

	IComponent(GameObject* const gameObject, Transform* const transform);
	virtual ~IComponent() = 0;

	virtual IComponent& operator=(IComponent&);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) = 0;

public:
	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) = 0;

	// IN: void
	// OUT: void
	//
	// Update that is called every frame
	virtual void Update();
	// IN: void
	// OUT: void
	//
	// Update only called every 1/60th of second
	virtual void FixedUpdate();
	// IN: void
	// OUT: void
	//
	// Update called only after every other update has been called
	virtual void LateUpdate();

	// IN: void
	// OUT: void
	//
	// Draws Gizmos for the component
	virtual void OnDrawGizmos();

	// IN: void
	// OUT: void
	//
	// Called on the first frame that collision has been detected
	virtual void OnCollisionEnter(Collider*);
	// IN: void
	// OUT: void
	//
	// Called on each sequential frame that collision is detected
	virtual void OnCollisionStay(Collider*);
	// IN: void
	// OUT: void
	//
	// Called on first frame that collision is no longer detected
	virtual void OnCollisionExit(Collider*);

	// IN: void
	// OUT: void
	//
	// Called on the first frame that collision with a trigger is detected
	virtual void OnTriggerEnter(Collider*);
	// IN: void
	// OUT: void
	//
	// Called each sequential frame after collision with a trigger is detected
	virtual void OnTriggerStay(Collider*);
	// IN: void
	// OUT: void
	//
	// Called when collision is no longer detected with a trigger
	virtual void OnTriggerExit(Collider*);

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable();
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable();

	virtual void OnDestroy();

	// IN: Sets if the component is enabled or not
	// OUT: void
	//
	// Set the enabled of the component
	void SetEnabled(bool enabled);

	// IN: void
	// OUT: bool - enabled of the component
	//
	// Returns the current status of the enabled of the component
	bool isEnabled() const;

	Transform* const transform;
	GameObject* const gameObject;

	friend class GameObject;
};

