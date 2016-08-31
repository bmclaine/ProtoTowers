//Author: William Zane Lewis
//
//Collectable resource that can be spent on towers/weapons
#include "stdafx.h"
#include "Resource.h"
#include "Player.h"
#include "Assets\Material.h"



Resource::Resource(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_itKillTime.SetTime(KILL_TIME);
	m_goTarget = nullptr;
	m_bIsTargeted = false;
	m_bIsDead = false;
	
}

Resource::~Resource()
{

}

void Resource::Init()
{

}

//DESCRIPTION: Resets the timer to max kill time
//
//IN: void
//
//OUT: void
void Resource::ResetKillTime()
{
	m_bIsDead = false;
	m_itKillTime.Reset();
	m_itKillTime.SetTime(KILL_TIME);
}

//DESCRIPTION: Checks kill time, updates the position if need be
//
//IN: void
//
//OUT: void
void Resource::Update()
{
	if (m_itKillTime.IsFinished())
	{
		Inferno::Destroy(gameObject);
	}
	if (m_goTarget != nullptr)
	{
		FlyToTarget();
	}

}

//DESCRIPTION: If it is triggered by a player, it flies to him and destoyes itself
//
//IN: Collider * - the object that triggered it
//
//OUT: void
void Resource::OnTriggerEnter(Collider * _col)
{
	if (!m_bIsTargeted)
	{
		m_goTarget = _col->gameObject->GetComponent<Player>();
		if (m_goTarget != nullptr)
		{
			gameObject->GetComponent<Rigidbody>()->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			m_bIsTargeted = true;
			m_itKillTime.Reset();
			m_itKillTime.SetTime(FLY_TIME);
			unsigned int vectorSize = (unsigned int)gameObject->GetComponents<SphereCollider>().size();
			for (unsigned int i = 0; i < vectorSize; i++)
			{
				SphereCollider * myCol = gameObject->GetComponents<SphereCollider>()[0];
				myCol->SetEnabled(false);

			}
			Inferno::PostAudioEvent(RESOURCE_PICK_UP);
		}
	}
}

 void Resource::OnEnable() 
{
	IComponent::OnEnable();
	ResetKillTime();
	m_bIsTargeted = false;
	m_bIsDead = false;
}

//DESCRIPTION: Moves the resource to the target's position
//
//IN: void
//
//OUT: void
void Resource::FlyToTarget()
{
	Vector3 vectorToTarget = m_goTarget->gameObject->transform->GetPosition() - transform->GetPosition();
	//Sets killtime to zero, destroys self on next pass
	if (vectorToTarget.MagnitudeSq() < MINIMUM_DISTANCE && !m_bIsDead)
	{
		m_bIsDead = true;
		Player::AddResources(VALUE);
		m_itKillTime.Reset();
		m_itKillTime.SetTime(0.0f);
		return;
	}
	float ratio = (FLY_TIME - m_itKillTime.DifferenceBetweenTheStartTimeAndTimePassedForBrian()) / FLY_TIME;
	vectorToTarget = vectorToTarget + Vector3(0.0f, VERTICAL_OFFSET, 0.0f);
	vectorToTarget = vectorToTarget * ratio;
	transform->Translate((vectorToTarget + Vector3(0, sin(fabs(vectorToTarget.x)) * 1.2f, 0)) * Time::DeltaTime * 300);
}

IComponent& Resource::operator=(IComponent& _rhs)
{
	Resource* rhsBT = dynamic_cast<Resource*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

Resource& Resource::operator=(const Resource& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_bIsTargeted = _rhs.m_bIsTargeted;
		m_goTarget = _rhs.m_goTarget;
		m_itKillTime = _rhs.m_itKillTime;
		m_bIsDead = _rhs.m_bIsDead;
	}

	return *this;
}

IComponent* Resource::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	Resource* newComponent = new Resource(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

void Resource::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}