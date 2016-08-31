#include "stdafx.h"
#include "StunEffect.h"
#include "AIEntity.h"
#include "Engine.h"

StunEffect::StunEffect(GameObject* const gameObject, Transform* const transform) : StatusEffect(gameObject, transform)
{
	m_pEntity = nullptr;
}

StunEffect::~StunEffect()
{
}

IComponent& StunEffect::operator=(IComponent& _rhs) 
{ 
	StunEffect* rhsSE = dynamic_cast<StunEffect*>(&_rhs);
	if (rhsSE == nullptr) {
		return *this;
	}
	
	return operator=(*rhsSE);
}

StunEffect& StunEffect::operator=(const StunEffect& rhs)
{
	StatusEffect::operator=(rhs);

	m_pEntity = rhs.m_pEntity;

	return *this;
}
IComponent* StunEffect::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	StunEffect* SE = new StunEffect(gameObject, transform);
	(*SE) = *this;

	return SE;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void StunEffect::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void StunEffect::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) {}

// IN: void
// OUT: void
//
// Update that is called every frame
void StunEffect::Update()
{
	if (m_pEntity)
	{
		Matrix4 Local = transform->LocalMatrix();
		m_pEntity->GetHeadBone(Local);
		transform->SetPosition(Local.Translation());
	}

	transform->Rotate(Vector3(0, IE_PIMUL2 * Time::DeltaTime, 0));
}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void StunEffect::OnEnable()
{
	m_pEntity = gameObject->GetComponentInParent<AIEntity>();

	if (m_pEntity && m_pEntity->GetState() != State::STUNNED)
	{
		m_PrevState = m_pEntity->GetState();
		m_pEntity->SetState(State::STUNNED);
		Inferno::PostAudioEventAtObjectPosition(DEFAULT_MINION_STUNNED, gameObject);
	}
	else
	{
		Inferno::Destroy(gameObject);
	}
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void StunEffect::OnDisable()
{
	if (m_pEntity)
		m_pEntity->SetState(m_PrevState);
}