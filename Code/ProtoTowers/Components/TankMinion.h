///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The tank minion inherits from AIEntity.
//              It will target anything close to him, but the player will be its highest priority.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes


// Engine Includes
#include "AIEntity.h"

class SkinnedMeshRenderer;
class UIElement;
class UIRenderer;
class Texture;

class TankMinion : public AIEntity
{
private:
	struct MapElement
	{
		Transform* transform;
		UIElement* element;
	};

	UIElement  *m_pIconElement;
	UIRenderer *m_pIconRenderer;
	Texture    *m_pIcon;
	MapElement  m_pMapElement;

	SkinnedMeshRenderer* m_pRenderer;

	virtual IComponent& operator = ( IComponent& _assign ) override;
	TankMinion& operator = (const TankMinion& _assign);
	IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform);

	void HandleParticles( void ) const override;

public:
	// Default
	TankMinion( GameObject* const _gameObject, Transform* const _transform );
	~TankMinion( void );


	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	// Update - Nothing special
	void Update(void) override;

	void TakeDamage(DamageInfo _damageInfo) override;
	void OnEnable( void ) override;
	void OnDisable( void ) override;
	void Death( void ) override;
	void FindClosestTarget() override;
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

	// IN: VOID
	//
	// OUT: Matrix4 - The matrix transform of this minions head bone
	//
	// Description: Returns the transform of this minions head bone.
	virtual void GetHeadBone(Matrix4& _matrix) override;
};

