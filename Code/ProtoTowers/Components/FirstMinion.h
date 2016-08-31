#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

class MiniMinion;
class CutScene;

class FirstMinion : public IComponent
{
	InfernoTimer m_itWaitAfterDeath;
	MiniMinion* m_pMinion;
	CutScene* m_pCutScene;
	int m_iChannel;

	FirstMinion& operator=(const FirstMinion& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	FirstMinion(GameObject* const gameObject, Transform* const transform);
	virtual ~FirstMinion() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable();

	// IN: void
	// OUT: void
	//
	// Update only called every 1/60th of second
	virtual void FixedUpdate() override;

	// IN: void
	// OUT: void
	//
	// Called on the first frame that collision with a trigger is detected
	virtual void OnTriggerEnter(Collider*);

	void OnDeathTimerFinish();
};

