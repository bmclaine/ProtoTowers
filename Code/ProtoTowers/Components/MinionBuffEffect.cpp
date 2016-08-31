#include "stdafx.h"
#include "MinionBuffEffect.h"
#include "AIEntity.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "Components\MeshRenderer.h"

MinionBuffEffect::MinionBuffEffect(GameObject* const gameObject, Transform* const transform) : StatusEffect(gameObject, transform)
{
	m_fDamageMultiplier = 1.25f;
	m_fHealthIncrease = 25.0f;
}

MinionBuffEffect::~MinionBuffEffect()
{
}

MinionBuffEffect& MinionBuffEffect::operator=(const MinionBuffEffect& rhs)
{
	return *this;
}
IComponent* MinionBuffEffect::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new MinionBuffEffect(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void MinionBuffEffect::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void MinionBuffEffect::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) {}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void MinionBuffEffect::OnEnable()
{
	AIEntity* entity = gameObject->GetComponentInParent<AIEntity>();

	if (entity)
	{
		entity->SetHealth(entity->GetHealth() + m_fHealthIncrease);
		entity->SetDamageMultiplier(m_fDamageMultiplier);
		entity->transform->SetScale(1.25f, 1.25f, 1.25f);
		
		MeshRenderer* rend = entity->gameObject->GetComponentInChildren<MeshRenderer>();

		if (rend)
			rend->SetColor(Color(1.0f, 0.5f, 0.5f, 1.0f));
	}
}
// IN: void
// OUT: void
//
// Called when the Component is being destroyed
void MinionBuffEffect::OnDestroy()
{
	AIEntity* entity = gameObject->GetComponentInParent<AIEntity>();

	if (entity)
	{
		if (entity->GetHealth() > entity->GetMaxHealth())
			entity->SetHealth(entity->GetMaxHealth());

		entity->SetDamageMultiplier(1.0f);
		entity->transform->SetScale(1.0f, 1.0f, 1.0f);

		MeshRenderer* rend = entity->gameObject->GetComponentInChildren<MeshRenderer>();

		if (rend)
			rend->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void MinionBuffEffect::SetHealthIncrease(float health)
{
	m_fHealthIncrease = health;
}
void MinionBuffEffect::SetDamageMultiplierIncrease(float mul)
{
	m_fDamageMultiplier = mul;
}