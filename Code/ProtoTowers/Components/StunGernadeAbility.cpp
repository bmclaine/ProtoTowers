#include "stdafx.h"
#include "StunGernadeAbility.h"
#include "InfernoTimer.h"
#include "StunEffect.h"
#include "Components\Transform.h"
#include "Components\ParticleSystem.h"
#include "../Core/PlayerController.h"
#include "Assets\Texture.h"
#include "AIEntity.h"

#include "ModuleInterface.h"
#include "Engine.h"

StunGrenadeAbility::StunGrenadeAbility(GameObject* const gameObject, Transform* const transform) : Ability(gameObject, transform)
{
	m_fRadius = 5.0f;
	m_fStunTime = 5.0f;
	m_fCooldown = 15.0f;
	m_bCanCast = true;

	m_pIndicatorCircle = nullptr;

	m_itCastTimer.SetOnTimerEndFunction(std::bind(&StunGrenadeAbility::OnCastFinished, this));
	m_itCooldownTimer.SetOnTimerEndFunction(std::bind(&StunGrenadeAbility::OnCooldownFinished, this));
}

StunGrenadeAbility::~StunGrenadeAbility()
{
}

IComponent& StunGrenadeAbility::operator=(IComponent&) { return *this; }
StunGrenadeAbility& StunGrenadeAbility::operator=(const StunGrenadeAbility& rhs)
{
	Ability::operator=(rhs);

	m_fRadius = rhs.m_fRadius;
	m_fStunTime = rhs.m_fStunTime;

	return *this;
}
IComponent* StunGrenadeAbility::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	StunGrenadeAbility* sga = new StunGrenadeAbility(gameObject, transform);
	(*sga) = *this;

	return sga;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void StunGrenadeAbility::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void StunGrenadeAbility::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) {

	Ability::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Radius";
	Property* prop = data->GetProperty(propName);
	/*m_fRadius = (float)atof(prop->value.c_str());
	m_fStunTime = (float)atof(prop->value.c_str());*/

	if (m_pTexture)
		delete m_pTexture;
	int image = Inferno::GetImage(L"StunGernadeAbilityIcon.dds");
	m_pTexture = new Texture();
	m_pTexture->SetImage(image);
}

void StunGrenadeAbility::OnEnable()
{
	//if (m_pController){
	//	m_pController->SetAbility(this, 0);
	//}

	if (m_pController)
		m_fCastTime = m_pController->GetAnimationTime(PlayerController::THROWABILITY);

	m_pIndicatorCircle = Inferno::Instantiate("IndicatorCircle");

	if (m_pIndicatorCircle)
	{
		m_pIndicatorCircle->transform->SetParent(transform);
		m_pIndicatorCircle->transform->SetPosition(0.0f, 0.01f, 5.0f);
		m_pIndicatorCircle->transform->SetScale(m_fRadius, m_fRadius, 1.0f);
		m_pIndicatorCircle->SetActive(false, true);
	}
}

// IN: void
// OUT: void
//
// Uses the ability
void StunGrenadeAbility::UseAbility()
{
	if (m_bCanCast)
	{
		m_bCanCast = false;

		if (m_pController)
			m_pController->SetIgnore(IGNORE_MOVEMENT | IGNORE_SHOOTING, true);

		if (m_fCastTime > 0.0f)
		{
			// Start throw animation.
			if (m_pController)
			{
				m_pController->SetState(PlayerController::THROWABILITY);
				m_pController->SetIgnoreInput(true);
			}

			m_itCastTimer.SetTime(m_fCastTime);
		}
		else
			OnCastFinished();
	}
}

// IN: bool - To show the indicator or not
// OUT: void
//
// Sets the Indicator Circles display status to the incoming bool
void StunGrenadeAbility::DisplayIndicatorCircle(bool display)
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
// To be called when the cast time is finished
void StunGrenadeAbility::OnCastFinished()
{
	ReenableControls();

	m_itCooldownTimer.SetTime(m_fCooldown);

	GameObject* stunGrenade = Inferno::Instantiate("Stun Grenade");

	if (stunGrenade)
	{
		Inferno::Destroy(stunGrenade, 0.5f);
		stunGrenade->transform->SetPosition(m_pIndicatorCircle->transform->GetPosition() /*transform->GetPosition() + transform->WorldMatrix().Forward() * 2.0f*/);
		ParticleSystem* pSys = stunGrenade->GetComponent<ParticleSystem>();

		if (pSys)
			pSys->Play();
	}

	std::vector<RayCastHit> collisions;
	InfernoEngine_Physics::SphereOverlap(stunGrenade->transform->GetPosition(), m_fRadius, collisions);

	Inferno::PostAudioEvent(ABILITY_FLASH_BANG);

	// Loop through each object that was hit by the stun grenade and see if it is an enemy minion.
	for (size_t obj = 0; obj < collisions.size(); obj++)
	{
		if (collisions[obj].collider->gameObject->GetLayer() == (UINT)CollisionLayers::LAYER_ENEMY)
		{
			AIEntity* entity = collisions[obj].collider->gameObject->GetComponent<AIEntity>();

			// If we did hit an enemy minion who isn't dying, apply the stun effect
			if (entity && !entity->IsDying() && !entity->IsBuffed())
			{
				GameObject* stun = Inferno::Instantiate("StunEffect");
				stun->transform->SetParent(entity->transform);
				StunEffect* effect = stun->GetComponent<StunEffect>();
				effect->SetEffectTime(m_fStunTime);
			}
		}
	}

	// Switch back to idle.
	if (m_pController)
	{
		m_pController->SetState(PlayerController::IDLE);
		m_pController->SetIgnoreInput(false);
	}
}

// IN: void
// OUT: void
//
// Called when the cooldown timer expires
void StunGrenadeAbility::OnCooldownFinished()
{
	m_bCanCast = true;
}