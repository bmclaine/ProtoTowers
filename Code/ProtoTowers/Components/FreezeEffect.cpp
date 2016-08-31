#include "stdafx.h"
#include "FreezeEffect.h"
#include "AIEntity.h"

#include "Components\Animator.h"
#include "EventSystem.h"

// ===== Constructor / Destructor ===== //
FreezeEffect::FreezeEffect(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_fDamageRate = 1.0f;
	m_fSpeedMultiplier = 0.6f;  // <-- Temp
	m_uiDamage = 0;
	m_pDamageTimer = new InfernoTimer();
	m_pNavAgent = nullptr;
	m_pPlayer = nullptr;
}

FreezeEffect::~FreezeEffect()
{
	delete m_pDamageTimer;
}
// ==================================== //

// ===== Interface ===== //
void FreezeEffect::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{
	ObjectData* data = _dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;

	propName = "SpeedMultiplier";
	prop = data->GetProperty(propName);
	SetSpeedMultipier((float)atof(prop->value.c_str()));

	propName = "DamageRate";
	prop = data->GetProperty(propName);
	SetDamageRate((float)atof(prop->value.c_str()));

	propName = "Damage";
	prop = data->GetProperty(propName);
	SetDamage((unsigned int)atoi(prop->value.c_str()));
}

void FreezeEffect::OnEnable()
{
	IComponent::OnEnable();

	if (transform->GetParent() == nullptr) {
		return;
	}

	m_pDamageTimer->SetTime(m_fDamageRate);
	m_pDamageTimer->Reset();
	m_pDamageTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&FreezeEffect::OnDealDamage, this));

	m_pEntity = transform->GetParent()->gameObject->GetComponent<Entity>();

	AIEntity* ai = transform->GetParent()->gameObject->GetComponent<AIEntity>();

	// === Start the Particles
	ParticleSystem* pSystem = gameObject->GetComponent<ParticleSystem>();
	if (pSystem != nullptr) {
		pSystem->Play();
	}

	Animator* animator = m_pEntity->gameObject->GetComponentInChildren<Animator>();
	float actualMultiplier = (ai && ai->IsBuffed()) ? 0.95f : m_fSpeedMultiplier;	//if the minion is buffed; the effect is less effective
	if (animator != nullptr) {
		animator->SetSpeed(actualMultiplier);
	}

	float prevSpeed;
	// === Is it a NavAgent?
	m_pNavAgent = transform->GetParent()->gameObject->GetComponent<NavAgent>();
	if (m_pNavAgent != nullptr) {
		prevSpeed = m_pNavAgent->GetSpeed();
		m_fDecreaseAmount = prevSpeed - (prevSpeed * actualMultiplier);
		m_pNavAgent->SetSpeed(prevSpeed * actualMultiplier);
		return;
	}

	// === Is it a Player?
	m_pPlayer = transform->GetParent()->gameObject->GetComponent<PlayerController>();
	if (m_pPlayer != nullptr) {
		prevSpeed = m_pPlayer->GetMovementSpeed();
		m_fDecreaseAmount = prevSpeed - (prevSpeed * m_fSpeedMultiplier);
		m_pPlayer->SetMovementSpeed(prevSpeed * m_fSpeedMultiplier);

		bool data = true;
		EventSystem::GetInstance()->SendEventNow("ToggleHUDFrost", (void*)&data);

		return;
	}
}

void FreezeEffect::OnDisable()
{
	m_pDamageTimer->SetTime(0.0f);
	m_pDamageTimer->Reset();

	IComponent::OnDisable();
}
// ===================== //

// === Protected Interface === //
IComponent& FreezeEffect::operator=(IComponent& _rhs)
{
	FreezeEffect* rhsFE = dynamic_cast<FreezeEffect*>(&_rhs);
	if (rhsFE == nullptr) {
		return *this;
	}

	return operator=(*rhsFE);
}

FreezeEffect& FreezeEffect::operator=(const FreezeEffect& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fDamageRate = _rhs.m_fDamageRate;
		m_fDecreaseAmount = _rhs.m_fDecreaseAmount;
		m_fSpeedMultiplier = _rhs.m_fSpeedMultiplier;
		m_uiDamage = _rhs.m_uiDamage;
		*m_pDamageTimer = *_rhs.m_pDamageTimer;
	}

	return *this;
}

IComponent* FreezeEffect::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	FreezeEffect* newFreezeEffect = new FreezeEffect(_gameObject, _transform);
	*newFreezeEffect = *this;

	return newFreezeEffect;
}
// =========================== //

// ===== Events ===== //
void FreezeEffect::OnDealDamage()
{
	m_pEntity->TakeDamage(DamageInfo((float)m_uiDamage));
	m_pDamageTimer->Reset();
}

void FreezeEffect::OnRemoveEffect()
{
	Animator* animator = m_pEntity->gameObject->GetComponentInChildren<Animator>();
	if (animator != nullptr) {
		animator->SetSpeed(1.0f);
	}

	// === Is it a NavAgent?
	if (m_pNavAgent != nullptr) {
		m_pNavAgent->SetSpeed(m_pNavAgent->GetSpeed() + m_fDecreaseAmount);
	}
	// === Is it a Player?
	else if (m_pPlayer != nullptr) {
		m_pPlayer->SetMovementSpeed(m_pPlayer->GetMovementSpeed() + m_fDecreaseAmount);

		bool data = false;
		EventSystem::GetInstance()->SendEventNow("ToggleHUDFrost", (void*)&data);
	}

	Inferno::Destroy(this->gameObject);
}
// ================== //

// ===== Accessors ===== //
float FreezeEffect::GetSpeedMultipier() const
{
	return m_fSpeedMultiplier;
}

float FreezeEffect::GetDamageRate() const
{
	return m_fDamageRate;
}

unsigned int FreezeEffect::GetDamage() const
{
	return m_uiDamage;
}
// ===================== //

// ===== Mutators ===== //
void FreezeEffect::SetSpeedMultipier(float _multi)
{
	m_fSpeedMultiplier = _multi;
}

void FreezeEffect::SetDamageRate(float _rate)
{
	m_fDamageRate = _rate / 1.0f;
}

void FreezeEffect::SetDamage(unsigned int _damage)
{
	m_uiDamage = _damage;
}
// ==================== //
