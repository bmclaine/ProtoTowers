//Author: William Zane Lewis
//
//Collectable resource that can be spent on towers/weapons
#pragma once
//Time before the resource destroyes itself
#define KILL_TIME 15.0f
//Time that the resource is in the air
#define FLY_TIME 7.5f
//The amount the resource is worth
#define VALUE 12
//Minimum distance between the player and the resource
#define MINIMUM_DISTANCE 3.5f
//Offset from ground so that the resource doesn't fly to the player's feet
#define VERTICAL_OFFSET 1.125f

class GameObject;
class InfernoTimer;
class IComponent;
class Player;
#include "Assets\Material.h"

class Resource : public IComponent
{
public:
	
	void Init();

	Resource(GameObject* const gameObject, Transform* const transform);
	
	virtual ~Resource()override;	
	
	IComponent& operator=(IComponent&) override;
	Resource& operator=(const Resource& _rhs);
	//DESCRIPTION: Moves the resource to the target's position
	//
	//IN: void
	//
	//OUT: void
	void OnTriggerEnter(Collider * _col) override;

	//DESCRIPTION: Resets the timer to max kill time
	//
	//IN: void
	//
	//OUT: void
	void ResetKillTime();

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable() override;

	//DESCRIPTION: Checks kill time, updates the position if need be
	//
	//IN: void
	//
	//OUT: void
	void Update() override;
protected:
	
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;
	//DESCRIPTION: Moves the resource to the target's position
	//
	//IN: void
	//
	//OUT: void
	void FlyToTarget();

	//Target for resource to fly to
	Player * m_goTarget;
	
	//bool to keep from triggering twice
	bool m_bIsTargeted;

	bool m_bIsDead;

	//Timer before the resource kills itself
	InfernoTimer m_itKillTime;

	


};