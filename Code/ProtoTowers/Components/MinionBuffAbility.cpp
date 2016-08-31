#include "stdafx.h"
#include "MinionBuffAbility.h"
#include "MinionBuffEffect.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "Components\ParticleSystem.h"
#include "Components\SpriteRenderer.h"
#include "AIEntity.h"

#include "ModuleInterface.h"
#include "Engine.h"

MinionBuffAbility::MinionBuffAbility(GameObject* const gameObject, Transform* const transform) : Ability(gameObject, transform)
{
	m_cActiveTimerColor = Color(1.0f, 0.95f, 0.0f, 1.0f);

	m_fCooldown = 30.0f;
	m_fBuffTime = 10.0f;
	m_fRadius = 10.0f;
	m_fHealthIncrease = 50.0f;
	m_fDamageMultiplier = 1.25f;

	m_pIndicatorCircle = nullptr;

	m_bCanCast = true;

	m_itCastTimer.SetOnTimerEndFunction(std::bind(&MinionBuffAbility::OnCastFinished, this));
	m_itCooldownTimer.SetOnTimerEndFunction(std::bind(&MinionBuffAbility::OnCooldownFinished, this));
}

MinionBuffAbility::~MinionBuffAbility()
{
}

MinionBuffAbility& MinionBuffAbility::operator=(const MinionBuffAbility& rhs)
{
	Ability::operator=(rhs);

	m_fBuffTime = rhs.m_fBuffTime;
	m_fRadius = rhs.m_fRadius;

	return *this;
}
IComponent* MinionBuffAbility::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	MinionBuffAbility* mba = new MinionBuffAbility(gameObject, transform);
	(*mba) = *this;

	return mba;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void MinionBuffAbility::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void MinionBuffAbility::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	Ability::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	if (m_pTexture)
		delete m_pTexture;

	int image = Inferno::GetImage(L"MinionBuff.dds");
	m_pTexture = new Texture();
	m_pTexture->SetImage(image);
}

void MinionBuffAbility::OnEnable()
{
	//if (m_pController){
	//	m_pController->SetAbility(this, 3);
	//}

	if (m_pController)
		m_fCastTime = m_pController->GetAnimationTime(PlayerController::BUTTONPUSH);

	m_pIndicatorCircle = Inferno::Instantiate("IndicatorCircle");

	if (m_pIndicatorCircle)
	{
		m_pIndicatorCircle->transform->SetParent(transform);
		m_pIndicatorCircle->transform->SetScale(m_fRadius, m_fRadius, 1.0f);
		m_pIndicatorCircle->SetActive(false, true);
	}
}

// IN: void
// OUT: void
//
// Uses the ability
void MinionBuffAbility::UseAbility()
{
	if (m_bCanCast)
	{
		m_bCanCast = false;

		if (m_pController)
			m_pController->SetIgnore(IGNORE_MOVEMENT | IGNORE_SHOOTING, true);

		if (m_fCastTime > 0.0f)
		{
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
void MinionBuffAbility::OnCastFinished()
{
	bool minionCollision = false;

	ReenableControls();

	m_itCooldownTimer.SetTime(m_fCooldown);

	GameObject* Wave = Inferno::Instantiate("MinionBuffWave");
	Wave->transform->SetParent(transform);
	Wave->transform->SetPosition(0, 0, 0);

	std::vector<RayCastHit> collisions;
	InfernoEngine_Physics::SphereOverlap(transform->GetPosition(), m_fRadius, collisions);
	Inferno::PostAudioEvent(ABILITY_MINION_BUFF);

	// Loop through each object that was hit by the stun grenade and see if it is an enemy minion.
	for (size_t obj = 0; obj < collisions.size(); obj++)
	{
		if (collisions[obj].collider->gameObject->GetLayer() == (UINT)CollisionLayers::LAYER_FRIENDLY)
		{
			AIEntity* entity = collisions[obj].collider->gameObject->GetComponent<AIEntity>();

			// If we did hit a friendly minion who isn't dying, apply the buff
			if (entity && !entity->IsDying())
			{
				GameObject* Buff = Inferno::Instantiate("MinionBuff");

				if (Buff)
				{
					MinionBuffEffect* effect = Buff->GetComponent<MinionBuffEffect>();
					effect->SetHealthIncrease(m_fHealthIncrease);
					effect->SetDamageMultiplierIncrease(m_fDamageMultiplier);
					effect->SetEffectTime(m_fBuffTime);

					ParticleSystem* pSys = Buff->GetComponent<ParticleSystem>();

					if (pSys)
						pSys->Play();

					Buff->transform->SetParent(entity->transform);

					minionCollision = true;
				}
			}
		}
	}

	if (minionCollision)
		m_pController->SetAbilityActiveTime(m_fBuffTime, 3);

	if (m_pController)
	{
		m_pController->SetState(PlayerController::IDLE);
		m_pController->SetIgnoreInput(false);
	}
}

// IN: bool - To show the indicator or not
// OUT: void
//
// Sets the Indicator Circles display status to the incoming bool
void MinionBuffAbility::DisplayIndicatorCircle(bool display)
{
	if (m_pIndicatorCircle)
	{
		if (m_bCanCast)
			m_pIndicatorCircle->SetActive(display);
		else
			m_pIndicatorCircle->SetActive(false);
	}
}

// IN: void
// OUT: void
//
// Called when the cooldown timer expires
void MinionBuffAbility::OnCooldownFinished()
{
	m_bCanCast = true;
}

float MinionBuffAbility::GetBuffTime() const
{
	return m_fBuffTime;
}

void MinionBuffAbility::SetBuffTime(float time)
{
	m_fBuffTime = time;
}