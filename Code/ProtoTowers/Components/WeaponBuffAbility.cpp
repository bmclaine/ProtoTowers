#include "stdafx.h"
#include "WeaponBuffAbility.h"
#include "WeaponBuffScript.h"
#include "Components\Transform.h"
#include "Assets/Texture.h"
#include "../../Interface/Public/Engine.h"

WeaponBuffAbility::WeaponBuffAbility(GameObject* const gameObject, Transform* const transform) : Ability(gameObject, transform)
{
	m_cActiveTimerColor = Color(1.0f, 0.0f, 0.0f, 1.0f);

	m_fCastTime = 0.5f;
	m_fCooldown = 25.0f;
	m_fDuration = 5.0f;
	m_fFireRateIncrease = FLT_MIN;

	m_bCanCast = true;

	m_itCastTimer.SetOnTimerEndFunction(std::bind(&WeaponBuffAbility::OnCastFinished, this));
	m_itCooldownTimer.SetOnTimerEndFunction(std::bind(&WeaponBuffAbility::OnCooldownFinished, this));
}

WeaponBuffAbility::~WeaponBuffAbility()
{
}

WeaponBuffAbility& WeaponBuffAbility::operator=(const WeaponBuffAbility& rhs)
{
	Ability::operator=(rhs);

	m_fDuration = rhs.m_fDuration;
	m_fFireRateIncrease = rhs.m_fFireRateIncrease;

	return *this;
}

IComponent* WeaponBuffAbility::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	WeaponBuffAbility* WBA = new WeaponBuffAbility(gameObject, transform);
	(*WBA) = *this;

	return WBA;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void WeaponBuffAbility::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void WeaponBuffAbility::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	Ability::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	if (m_pTexture)
		delete m_pTexture;

	int image = Inferno::GetImage(L"WeaponBuff.dds");
	m_pTexture = new Texture();
	m_pTexture->SetImage(image);
}

void WeaponBuffAbility::OnEnable()
{
	//if (m_pController){
	//	m_pController->SetAbility(this, 2);
	//}
}

// IN: void
// OUT: void
//
// Uses the ability
void WeaponBuffAbility::UseAbility()
{
	if (m_bCanCast)
	{
		m_bCanCast = false;

		Inferno::PostAudioEvent(ABILITY_WEAPON_BUFF);

		if (m_pController)
			m_pController->SetIgnore(IGNORE_SHOOTING, true);

		if (m_fCastTime > 0.0f)
			m_itCastTimer.SetTime(m_fCastTime);
		else
			OnCastFinished();
	}
}

// IN: void
// OUT: void
//
// To be called when the cast time is finished
void WeaponBuffAbility::OnCastFinished()
{
	ReenableControls();

	m_itCooldownTimer.SetTime(m_fCooldown);

	GameObject* player = Inferno::GetGameObjectByName(std::string("Player"));

	GameObject* buff = Inferno::Instantiate("WeaponBuff");
	
	if (buff && player)
	{
		buff->transform->SetParent(player->transform);
		WeaponBuffScript* script = buff->GetComponent<WeaponBuffScript>();
		script->SetBuffTime(m_fDuration);
		script->SetShotSpeedIncrease(m_fFireRateIncrease);
	}

	if (m_pController)
		m_pController->SetAbilityActiveTime(m_fDuration, 2);
}
// IN: void
// OUT: void
//
// Called when the cooldown timer expires
void WeaponBuffAbility::OnCooldownFinished()
{
	m_bCanCast = true;
}