#include "stdafx.h"
#include "ShieldAbility.h"
#include "PlayerShieldScript.h"
#include "Assets\Texture.h"
#include "../../Interface/Public/Engine.h"

ShieldAbility::ShieldAbility(GameObject* const gameObject, Transform* const transform) : Ability(gameObject, transform)
{
	m_cActiveTimerColor = Color(0.0f, 0.0f, 1.0f, 1.0f);

	m_fCooldown = 30.0f;
	m_fRadius = 1.25f;
	m_fShieldTime = 5.0f;
	m_fExpandTime = 0.5f;

	m_bCanCast = true;

	m_itCastTimer.SetOnTimerEndFunction(std::bind(&ShieldAbility::OnCastFinished, this));
	m_itCooldownTimer.SetOnTimerEndFunction(std::bind(&ShieldAbility::OnCooldownFinished, this));
}

ShieldAbility::~ShieldAbility()
{
}

ShieldAbility& ShieldAbility::operator=(const ShieldAbility& rhs)
{
	Ability::operator=(rhs);

	m_fRadius = rhs.m_fRadius;
	m_fExpandTime = rhs.m_fExpandTime;
	m_fShieldTime = rhs.m_fShieldTime;

	return *this;
}
IComponent* ShieldAbility::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	ShieldAbility* SA = new ShieldAbility(gameObject, transform);
	(*SA) = *this;

	return SA;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void ShieldAbility::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void ShieldAbility::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	Ability::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	if (m_pTexture)
		delete m_pTexture;
	int image = Inferno::GetImage(L"PlayerShield.dds");
	m_pTexture = new Texture();
	m_pTexture->SetImage(image);
}

void ShieldAbility::OnEnable()
{
	//if (m_pController){
	//	m_pController->SetAbility(this, 1);
	//}

	if (m_pController)
		m_fCastTime = m_pController->GetAnimationTime(PlayerController::BUTTONPUSH);
}

// IN: void
// OUT: void
//
// Uses the ability
void ShieldAbility::UseAbility()
{
	if (m_bCanCast)
	{
		m_bCanCast = false;

		if (m_pController)
			m_pController->SetIgnore(IGNORE_SHOOTING, true);

		if (m_fCastTime > 0.0f)
		{
			// Start push animation.
			if (m_pController)
			{
				m_pController->SetState(PlayerController::BUTTONPUSH);
				m_pController->SetIgnoreInput(true);
			}

			m_itCastTimer.SetTime(m_fCastTime);
		}
		else
			OnCastFinished();
	}
}

// IN: void
// OUT: void
//
// To be called when the cast time is finished
void ShieldAbility::OnCastFinished()
{
	ReenableControls();

	m_itCooldownTimer.SetTime(m_fCooldown);

	GameObject* player = Inferno::GetGameObjectByName(std::string("Player"));

	GameObject* shieldObj = Inferno::Instantiate("PlayerShield");

	if (player)
		shieldObj->transform->SetParent(player->transform);

	PlayerShieldScript* shield = shieldObj->GetComponent<PlayerShieldScript>();
	shield->SetExpandTime(m_fExpandTime);
	shield->SetRadius(m_fRadius);
	shield->SetShieldTime(m_fShieldTime);

	Player* playerScript = m_pController->gameObject->GetComponent<Player>();
	if (playerScript)
		shield->SetPlayer(playerScript);

	Inferno::PostAudioEvent(ABILITY_SHIELD_UP);

	if (m_pController)
	{
		m_pController->SetAbilityActiveTime(m_fShieldTime, 1);
		m_pController->SetState(PlayerController::IDLE);
		m_pController->SetIgnoreInput(false);
	}
}
// IN: void
// OUT: void
//
// Called when the cooldown timer expires
void ShieldAbility::OnCooldownFinished()
{
	m_bCanCast = true;
}