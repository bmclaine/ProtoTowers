///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: RangedMinion inherits from AIEntity.
//              It will shoot projectiles at its target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes


// Engine Includes
#include "AIEntity.h"


class RangedMinion : public AIEntity
{
private:
	void Death( void ) override;
	virtual IComponent& operator = ( IComponent& _assign ) override;
	RangedMinion& operator = ( const RangedMinion& _assign );
	IComponent* CreateCopy( GameObject* const _gameObject, Transform* const _transform );

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Notifies Entity class to stop tracking an enemy
	void StopTrackingEnemy(Entity* entity);

public:
	// Default
	RangedMinion(GameObject* const _gameObject, Transform* const _transform);
	~RangedMinion(void);


	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	// Update - Nothing special
	void Update(void) override;

	void FindClosestTarget() override;

	void TakeDamage(DamageInfo _damageInfo) override;
	AIEntityType GetType() const override;


	// IN: _collider - The collider that triggered you
	//
	// OUT: VOID
	//
	// Description: When a collider triggers you, this will be called
	void OnTriggerEnter(Collider* _collider) override;

	// IN: _collider - The collider that left your trigger
	//
	// OUT: VOID
	//
	// Description: When a collider leaves your trigger, this function is called
	void OnTriggerExit(Collider* _collider) override;
};

