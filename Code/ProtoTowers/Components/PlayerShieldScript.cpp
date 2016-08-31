#include "stdafx.h"
#include "PlayerShieldScript.h"
#include "Engine.h"
#include "Player.h"

PlayerShieldScript::PlayerShieldScript(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_fAliveTime = 0.0f;
	m_fExpandTime = 0.0f;
	m_fRadius = 0.0f;
	m_fOffset = 1.5f;

	m_pPlayer = nullptr;
}

PlayerShieldScript::~PlayerShieldScript()
{
}

PlayerShieldScript& PlayerShieldScript::operator=(const PlayerShieldScript& rhs)
{
	return *this;
}
IComponent* PlayerShieldScript::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	PlayerShieldScript* PSS = new PlayerShieldScript(gameObject, transform);
	(*PSS) = *this;

	return PSS;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void PlayerShieldScript::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void PlayerShieldScript::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void PlayerShieldScript::OnEnable()
{
	Inferno::Destroy(gameObject, m_fAliveTime);
	m_itEpandTimer.SetTime(m_fExpandTime);

	transform->SetScale(0.01f, 0.01f, 0.01f);

	Vector3 pos = transform->LocalPosition();
	pos += Vector3(0.0f, m_fOffset, 0.0f);
	transform->SetPosition(pos);

	if (m_pPlayer)
		m_pPlayer->SetInvurnerable(true);
}

void PlayerShieldScript::OnDisable()
{
	if (m_pPlayer)
		m_pPlayer->SetInvurnerable(false);

	Inferno::PostAudioEvent(ABILITY_SHIELD_DOWN);
}

// IN: void
// OUT: void
//
// Update only called every 1/60th of second
void PlayerShieldScript::FixedUpdate()
{
	float size;

	if (m_itEpandTimer.IsFinished())
		size = m_fRadius;
	else
		size = 0.01f + ((m_fRadius - 0.01f) * (1.0f - (m_itEpandTimer.TimeRemaining() / m_itEpandTimer.GetSetTimeSeconds())));

	transform->SetScale(size, size, size);
}

// IN: void
// OUT: void
//
// Called on the first frame that collision has been detected
void PlayerShieldScript::OnCollisionEnter(Collider* col)
{
	Inferno::PostAudioEvent(ABILITY_SHIELD_HIT);
}

void PlayerShieldScript::SetRadius(float radius) { m_fRadius = radius; }
void PlayerShieldScript::SetExpandTime(float time) { m_fExpandTime = time; }
void PlayerShieldScript::SetShieldTime(float time) { m_fAliveTime = time; }
void PlayerShieldScript::SetOffset(float offset) { m_fOffset = offset; }
void PlayerShieldScript::SetPlayer(Player* player) { m_pPlayer = player; }