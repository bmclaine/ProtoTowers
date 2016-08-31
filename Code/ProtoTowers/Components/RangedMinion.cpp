///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: RangedMinion inherits from AIEntity.
//              It will shoot projectiles at its target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes


// Engine Includes
#include "stdafx.h"
#include "RangedMinion.h"
#include "ModuleInterface.h"
#include "MathLib.h"

RangedMinion::RangedMinion(GameObject* const _gameObject, Transform* const _transform) : AIEntity(_gameObject, _transform)
{
	SetAttackTimer(RANGED_MINION_ATTACK_TIME);
	m_pEnemyToTrack = nullptr;
	m_fMaxHealth = RANGED_MINION_HEALTH;
	m_fHealth = m_fMaxHealth;

	InfernoEngine_AI::RegisterAIEntity(this);
}

RangedMinion::~RangedMinion()
{
	InfernoEngine_AI::UnregisterAIEntity(this);
}

void RangedMinion::FindClosestTarget()
{
	
}

void RangedMinion::Death() /*override*/
{
	// TODO: Play death animation and call destroy on the object after its playing time

	AIEntity::Death();

	Inferno::Destroy(this->gameObject, 0.0001f);
}

/*virtual*/ IComponent& RangedMinion::operator = (IComponent& _assign) /*override*/
{
	RangedMinion* ranged = dynamic_cast<RangedMinion*>(&_assign);
	if (ranged == nullptr)
	{
		return *this;
	}

	return operator=(*ranged);
}

RangedMinion& RangedMinion::operator = (const RangedMinion& _assign)
{
	if (this != &_assign)
	{
		AIEntity::operator=(_assign);
	}

	return *this;
}

IComponent* RangedMinion::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	RangedMinion* temp = new RangedMinion(_gameObject, _transform);
	*temp = *this;

	return temp;
}

// IN: VOID
//
// OUT: VOID
//
// Description: Notifies Entity class to stop tracking an enemy
void RangedMinion::StopTrackingEnemy(Entity* entity)
{
	m_pEnemyToTrack = nullptr;
}


// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
/*virtual*/ void RangedMinion::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	AIEntity::PostInit(objectMap, dataMap);
}

// Update - Nothing special
void RangedMinion::Update() /*override*/
{
	// Check container of "nearby enemies" to attempt to find a "better" target, if any
	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	float distance = FLT_MAX;
	for (unsigned int i = 0; i < size; ++i)
	{
		float temp = (transform->GetPosition() - m_vNearbyEnemies[i]->transform->GetPosition()).MagnitudeSq();
		if (temp < distance)
		{
			distance = temp;
			//			m_vNearbyEnemies[i]->RemoveFromTracking(this);      <-- Why is this needed?
			m_pEnemyToTrack = m_vNearbyEnemies[i];
			m_pEnemyToTrack->AddForTracking(this, BindEntityOnReleaseFunction(&RangedMinion::StopTrackingEnemy, this));
		}
	}

	// If you're not tracking an enemy, return
	if (m_pEnemyToTrack == nullptr)
		return;

	Vector3 myPosition = transform->GetPosition();
	Vector3 playerPos = m_pEnemyToTrack->transform->GetPosition();
	distance = (myPosition - playerPos).MagnitudeSq();

	// If the player is within range and the minion can attack
	if (Time::DeltaTime > 0 && distance < RANGED_MINION_ATTACK_DIST && m_attackTimer.IsFinished())
	{
		m_attackTimer.Reset();

		// TODO: ANIMATION
	}
}

void RangedMinion::TakeDamage(DamageInfo _damageInfo) /*override*/
{
	m_fHealth -= _damageInfo.IncomingDamage;

	// The minion is dead
	if (m_fHealth <= 0)
	{
		Death();
	}
	// The minion is still alive
	else
	{
		// TODO: Play "take damage" animation
	}
}


// IN: _collider - The collider that triggered you
//
// OUT: VOID
//
// Description: When a collider triggers you, this will be called
void RangedMinion::OnTriggerEnter(Collider* _collider) /*override*/
{
	Entity* entity = _collider->gameObject->GetComponent<Entity>();
	if (entity == nullptr)
		return;
	if (entity->GetTeam() == NO_TEAM)
		return;

	if (entity->GetTeam() != m_eTeam)
	{
		unsigned int size = (unsigned int)m_vNearbyEnemies.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (m_vNearbyEnemies[i] == entity)
				return;
		}
		m_vNearbyEnemies.push_back(entity);
	}
}

// IN: _collider - The collider that left your trigger
//
// OUT: VOID
//
// Description: When a collider leaves your trigger, this function is called
void RangedMinion::OnTriggerExit(Collider* _collider) /*override*/
{
	AIEntity* entity = _collider->gameObject->GetComponent<AIEntity>();
	if (entity == nullptr)
		return;

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == entity)
		{
			m_vNearbyEnemies.erase(m_vNearbyEnemies.begin() + i);
			return;
		}
	}
}

AIEntityType RangedMinion::GetType() const
{
	return AIEntityType::TYPE_RANGED;
}

