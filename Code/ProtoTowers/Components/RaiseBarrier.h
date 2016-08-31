#pragma once

#include "Components\IComponent.h"

class MinionSpawner;

class RaiseBarrier : public IComponent
{
private:
	GameObject* m_pBarrierObject;
	GameObject* m_pSpawnPointObject;
	Transform* m_pNewSpawnPoint;
	std::vector<MinionSpawner*> m_vSpawners;

	RaiseBarrier& operator=(const RaiseBarrier& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	RaiseBarrier(GameObject* const gameObject, Transform* const transform);
	~RaiseBarrier();

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

	void OnEnable() override;

	// IN: void
	// OUT: void
	//
	// Called on the first frame that collision with a trigger is detected
	virtual void OnTriggerEnter(Collider*);
};

