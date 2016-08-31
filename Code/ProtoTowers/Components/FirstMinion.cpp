#include "stdafx.h"
#include "FirstMinion.h"
#include "MiniMinion.h"
#include "EventSystem.h"

#define WAIT_AFTER_KILL 2.0f

FirstMinion::FirstMinion(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_itWaitAfterDeath.SetOnTimerEndFunction(std::bind(&FirstMinion::OnDeathTimerFinish, this));
	m_pMinion = nullptr;
}

FirstMinion::~FirstMinion()
{
}

FirstMinion& FirstMinion::operator=(const FirstMinion& rhs)
{
	return *this;
}
IComponent* FirstMinion::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new FirstMinion(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void FirstMinion::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void FirstMinion::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Channel";
	Property* prop = data->GetProperty(propName);
	m_iChannel = atoi(prop->value.c_str());
}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void FirstMinion::OnEnable()
{
	// Since the prefab is never truly deleted and recreated, we need to reset the minion pointer and timer to ensure we can
	// continously do the tutorial over again.
	m_pMinion = nullptr;
	m_itWaitAfterDeath.FullReset();
}

// IN: void
// OUT: void
//
// Update only called every 1/60th of second
void FirstMinion::FixedUpdate()
{
	if (m_pMinion && m_pMinion->IsDying() && m_itWaitAfterDeath.GetSetTimeSeconds() == 0.0f)
	{
		m_itWaitAfterDeath.SetTime(WAIT_AFTER_KILL);
	}
}

// IN: void
// OUT: void
//
// Called on the first frame that collision with a trigger is detected
void FirstMinion::OnTriggerEnter(Collider* col)
{
	if (m_pMinion)
		return;

	m_pMinion = col->gameObject->GetComponent<MiniMinion>();
	
	if (m_pMinion)
		m_pMinion->SetState(State::STATIONARY);
}

void FirstMinion::OnDeathTimerFinish()
{
	EventSystem::GetInstance()->SendEventNow("BarrierSetActiveFalse", (void*)m_iChannel);
	Inferno::Destroy(gameObject);
}