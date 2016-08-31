///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The boss minion that inherits from AIEntity. Will be the hardest AIAgent to kill
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// STL Includes
#include <functional>

// Engine Includes
#include "AIEntity.h"

class GameObject;
class Resource;
class MainBase;
class UIElement;
class UIRenderer;
class Texture;
class SkinnedMeshRenderer;

// === TypeDefs
typedef std::function<void(float)> OnTakeDamageFunc;

class BossMinion : public AIEntity
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
	OnTakeDamageFunc m_OnTakeDamage;

	Vector3 m_vecPrevPosition;

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	BossMinion& operator= (const BossMinion& _rhs);
	IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform);
	void OnEnable(void) override;
	void OnDisable(void) override;
	void HandleParticles(void) const override;

public:
	// Defaults
	BossMinion(GameObject* const _gameObject, Transform* const _transform);
	~BossMinion(void);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	void TakeDamage(DamageInfo _damageInfo) override;
	void FindClosestTarget() override;
	void Update(void) override;
	void Death(void) override;
	unsigned int GetNumEnemiesWithinRange() const;
	void DamageNearbyEnemies();
	float GetDistanceTraveled();
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

	void SetOnTakeDamageFunction(OnTakeDamageFunc _func);
};

