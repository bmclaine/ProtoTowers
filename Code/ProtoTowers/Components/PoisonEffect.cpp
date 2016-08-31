#include "stdafx.h"
#include "PoisonEffect.h"
#include "AIEntity.h"

// === Defines
#define EFFECT_TIME 2.0f
#define DAMAGE_TIME 0.8f

// === Constructor / Destructor === //
PoisonEffect::PoisonEffect(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pDamageTimer = new InfernoTimer();
	m_pDamageTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&PoisonEffect::OnDealDamage, this));
	m_pEffectTimer = new InfernoTimer();
	m_pEffectTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&PoisonEffect::OnRemoveEffect, this));

	m_fDamage = 5.0f;
	m_uiInfectedCount = 1;
}

PoisonEffect::~PoisonEffect()
{
	delete m_pDamageTimer;
	delete m_pEffectTimer;
}
// ================================ //

// === Interface === //
void PoisonEffect::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	// === Nothing is Needed from Prefab File
}

void PoisonEffect::OnEnable()
{
	// === Make sure we were attached to something
	if (transform->GetParent() == nullptr) {
		Inferno::Destroy(this->gameObject);
		return;
	}

	// === Get the Target Entity
	m_pEntity = transform->GetParent()->gameObject->GetComponent<Entity>();
	if (m_pEntity == nullptr) {
		Inferno::Destroy(this->gameObject);
	}
	m_pEntity->AddForTracking(this, BindEntityOnReleaseFunction(&PoisonEffect::OnEntityRelease, this));

	m_pDamageTimer->SetTime(DAMAGE_TIME);
	m_pDamageTimer->Reset();

	m_uiInfectedCount = 1;
}

void PoisonEffect::OnDisable()
{
	// === Stop the Timers
	m_pDamageTimer->SetTime(0.0f);
	m_pDamageTimer->Reset();
	m_pEffectTimer->SetTime(0.0f);
	m_pEffectTimer->Reset();

	m_pEntity = nullptr;

//	OnRemoveEffect();

	IComponent::OnDisable();
}

void PoisonEffect::StartEffectTimer()
{
	if (m_uiInfectedCount > 0) {
		--m_uiInfectedCount;
	}
	
	// === Are there no more towers infecting us?
	if (m_uiInfectedCount == 0) {
		// === Start the Timers
		m_pDamageTimer->SetTime(DAMAGE_TIME);
		m_pDamageTimer->Reset();
		m_pEffectTimer->SetTime(EFFECT_TIME);
		m_pEffectTimer->Reset();
	}
}

void PoisonEffect::StopEffectTimer()
{
	// === Stop the Timers
	m_pEffectTimer->SetTime(0.0f);
	m_pEffectTimer->Reset();

	++m_uiInfectedCount;
}

void PoisonEffect::ResetEffectTimer()
{
	m_pEffectTimer->Reset();
}
// ================= //

// === Protected Interface === //
IComponent& PoisonEffect::operator=(IComponent& _rhs)
{
	PoisonEffect* rhsPE = dynamic_cast<PoisonEffect*>(&_rhs);
	if (rhsPE == nullptr) {
		return *this;
	}

	return operator=(*rhsPE);
}

PoisonEffect& PoisonEffect::operator=(const PoisonEffect& _rhs)
{
	if (this != &_rhs) {
		m_pDamageTimer->SetTime(_rhs.m_pDamageTimer->GetSetTimeSeconds());
		m_pEffectTimer->SetTime(_rhs.m_pEffectTimer->GetSetTimeSeconds());
		m_fDamage = _rhs.m_fDamage;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}

IComponent* PoisonEffect::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PoisonEffect* newPE = new PoisonEffect(_gameObject, _transform);
	*newPE = *this;

	return newPE;
}
// =========================== //

// === Private Interface === //
void PoisonEffect::OnDealDamage()
{
	if (m_pEntity == nullptr) {
		return;
	}

	AIEntity* ai = m_pEntity->gameObject->GetComponent<AIEntity>();

	float damage = m_fDamage * ((ai != nullptr && ai->IsBuffed()) ? 0.5f : 1.0f);
	m_pEntity->TakeDamage(DamageInfo(damage));
	m_pDamageTimer->Reset();
}

void PoisonEffect::OnRemoveEffect()
{
	Inferno::Destroy(this->gameObject);
}

void PoisonEffect::OnEntityRelease(Entity* _entity)
{
	transform->SetParent(nullptr);

	OnRemoveEffect();
}
// ========================= //

// === Mutators === //
void PoisonEffect::SetDamage(float _damage)
{
	m_fDamage = _damage;
}
// ================ //