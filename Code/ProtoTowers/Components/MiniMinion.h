///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Child class of Entity. Responsible for the MiniMinion actions.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// Engine Includes
#include "AIEntity.h"

// Forward Declaration
class GameObject;
class SkinnedMeshRenderer;
class BehaviorTree;

class MiniMinion : public AIEntity
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

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	MiniMinion& operator = ( const MiniMinion& _rhs);
	IComponent* CreateCopy( GameObject* const _gameObject, Transform* const _transform );

	// Overridden function from AIEntity
	void HandleParticles(void) const override;

public:
	// Default
	MiniMinion(GameObject* const _gameObject, Transform* const _transform);
	~MiniMinion( void );

	// Override from the parent
	void OnEnable( void ) override;

	void OnDisable(void) override;

	void FindClosestTarget() override;
	AIEntityType GetType() const override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	// Update - Nothing special
	void Update( void ) override;

	void TakeDamage(DamageInfo _damageInfo) override;
	void Death(void) override;

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

