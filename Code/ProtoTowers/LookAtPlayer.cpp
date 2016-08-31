#include "stdafx.h"
#include "LookAtPlayer.h"


LookAtPlayer::LookAtPlayer(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pPlayer = nullptr;
}

LookAtPlayer::~LookAtPlayer()
{
}

IComponent& LookAtPlayer::operator=(IComponent&)
{
	return *this;
}

IComponent* LookAtPlayer::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	LookAtPlayer* newLAP = new LookAtPlayer(gameObject, transform);
	(*newLAP) = *this;

	return newLAP;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void LookAtPlayer::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}

void LookAtPlayer::Update()
{
	if (m_pPlayer)
	{
		transform->LookAt(m_pPlayer->transform->GetPosition());

		//if ((transform->GetPosition() - m_pPlayer->transform->GetPosition()).Length() > 1)
		//	transform->Translate(Vector3(0, 0, 1 * Time::DeltaTime));
	}
}

void LookAtPlayer::SetPlayer(GameObject* player) { m_pPlayer = player; }