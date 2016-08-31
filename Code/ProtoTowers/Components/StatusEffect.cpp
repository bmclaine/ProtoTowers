#include "stdafx.h"
#include "StatusEffect.h"
#include "Engine.h"

StatusEffect::StatusEffect(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_itEffectTime.SetOnTimerEndFunction(std::bind(&StatusEffect::RemoveEffect, this));
}

StatusEffect::~StatusEffect()
{
}

StatusEffect& StatusEffect::operator=(const StatusEffect& rhs)
{
	m_itEffectTime.SetTime(rhs.m_itEffectTime.GetSetTimeSeconds());

	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void StatusEffect::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void StatusEffect::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) {}

// IN: void
// OUT: void
//
// Called whenever the effect needs to be removed
void StatusEffect::RemoveEffect()
{
	Inferno::Destroy(gameObject);
}

void StatusEffect::SetEffectTime(float time)
{
	m_itEffectTime.SetTime(time);
}