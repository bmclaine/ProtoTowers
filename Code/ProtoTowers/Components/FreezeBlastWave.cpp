#include "stdafx.h"
#include "FreezeBlastWave.h"

#include "../Core/PlayerController.h"
#include "Components\Animator.h"
#include "Components\AIEntity.h"
#include "Components\NavAgent.h"
#include "Components\Player.h"

// === Defines
#define GROWTH_TIME 1.0f
#define TOTAL_TIME 2.0f

// ===== Constructor / Destructor ===== //
FreezeBlastWave::FreezeBlastWave(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pMeshTransform = nullptr;
	m_pTimer = new InfernoTimer();
	m_pRenderer = nullptr;
	m_pFrozenPlayer = new FrozenPlayer();
	m_pCollider = nullptr;

	m_fStartScale = 0.2f;
	m_fEndScale = 7.0f;
}

FreezeBlastWave::~FreezeBlastWave()
{
	delete m_pTimer;
	delete m_pFrozenPlayer;
}
// ==================================== //

// ===== Interface ===== //
void FreezeBlastWave::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Damage";
	Property* prop = data->GetProperty(propName);

	m_fDamage = (float)atof(prop->value.c_str());
}

void FreezeBlastWave::OnEnable()
{
	m_bExpanding = true;

	m_pTimer->SetTime(GROWTH_TIME);
	m_pTimer->Reset();
	m_pTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&FreezeBlastWave::OnMeshGrowthDone, this));

	if (m_pMeshTransform == nullptr) {
		m_pMeshTransform = transform->GetChild(0);
	}

	if (m_pRenderer == nullptr) {
		m_pRenderer = gameObject->GetComponentInChildren<MeshRenderer>();
	}

	if (m_pCollider == nullptr) {
		m_pCollider = gameObject->GetComponent<SphereCollider>();
	}

	Inferno::PostAudioEventAtObjectPosition(WWiseIDs::FROZEN_EFFECT, this->gameObject);
}

void FreezeBlastWave::OnDisable()
{
	m_pTimer->SetTime(0.0f);
	m_pTimer->Reset();

	IComponent::OnDisable();
}

void FreezeBlastWave::Update()
{
	if (m_bExpanding) {
		float ratio = (GROWTH_TIME - m_pTimer->TimeRemaining()) / GROWTH_TIME;
		float newScale = ((m_fEndScale - m_fStartScale) * ratio) + m_fStartScale;
		m_pMeshTransform->SetScale(newScale, 0.75f, newScale);
	}
}
// ===================== //

// === Protected Interface === //
IComponent& FreezeBlastWave::operator=(IComponent& _rhs)
{
	FreezeBlastWave* rhsFBW = dynamic_cast<FreezeBlastWave*>(&_rhs);
	if (rhsFBW == nullptr) {
		return *this;
	}

	return operator=(*rhsFBW);
}

FreezeBlastWave& FreezeBlastWave::operator=(const FreezeBlastWave& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fDamage = _rhs.m_fDamage;
		m_fEndScale = _rhs.m_fEndScale;
		m_fStartScale = _rhs.m_fStartScale;
	}

	return *this;
}

IComponent* FreezeBlastWave::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	FreezeBlastWave* newFBW = new FreezeBlastWave(gameObject, transform);
	*newFBW = *this;

	return newFBW;
}
// =========================== //

// ===== Events ===== //
void FreezeBlastWave::OnEntityRelease(Entity* _entity)
{
	// === Is it a Minion?
	NavAgent* nAgent = _entity->gameObject->GetComponent<NavAgent>();
	if (nAgent != nullptr) {
		size_t count = m_vFrozenMinions.size();
		for (size_t i = 0; i < count; ++i) {
			if (m_vFrozenMinions[i].NAgent == nAgent) {
				std::iter_swap(m_vFrozenMinions.begin() + i, m_vFrozenMinions.end() - 1);
				m_vFrozenMinions.erase(m_vFrozenMinions.begin() + i);
				return;
			}
		}
	}

	// === Is it a Player?
	PlayerController* pController = _entity->gameObject->GetComponent<PlayerController>();
	if (pController != nullptr) {
		m_pFrozenPlayer->PController->PauseAnimations(false);
		m_pFrozenPlayer->PController = nullptr;
		return;
	}
}

void FreezeBlastWave::OnMeshGrowthDone()
{
	m_pTimer->SetTime(TOTAL_TIME - GROWTH_TIME);
	m_pTimer->Reset();
	m_pTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&FreezeBlastWave::OnLifetimeDone, this));

	m_bExpanding = false;

	m_pRenderer->SetEnabled(false);

	m_pCollider->SetEnabled(false);
}

void FreezeBlastWave::OnLifetimeDone()
{
	m_pTimer->SetTime(0.0f);

	// === UnFreeze AI
	size_t count = m_vFrozenMinions.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vFrozenMinions[i].NAgent->GetSpeed() == 0.0f) {
			m_vFrozenMinions[i].NAgent->SetSpeed(m_vFrozenMinions[i].PrevSpeed);
			m_vFrozenMinions[i].Animator->SetSpeed(m_vFrozenMinions[i].PrevAnimSpeed);
		}
		else {
			m_vFrozenMinions[i].NAgent->SetSpeed(m_vFrozenMinions[i].NAgent->GetSpeed() + m_vFrozenMinions[i].PrevSpeed);
			m_vFrozenMinions[i].Animator->SetSpeed(m_vFrozenMinions[i].Animator->GetSpeed() + m_vFrozenMinions[i].PrevAnimSpeed);
		}

		m_vFrozenMinions[i].Entity->SetState(State::AGGRO);
		m_vFrozenMinions[i].Entity->RemoveFromTracking(this);
		m_vFrozenMinions[i].Entity->TakeDamage(DamageInfo(m_fDamage));
	}
	m_vFrozenMinions.clear();

	// === UnFreeze Player
	if (m_pFrozenPlayer->PController != nullptr) {
		if (m_pFrozenPlayer->PController->GetMovementSpeed() == 0.0f) {
			m_pFrozenPlayer->PController->SetMovementSpeed(m_pFrozenPlayer->PrevSpeed);
			m_pFrozenPlayer->Animator->SetSpeed(m_pFrozenPlayer->PrevAnimSpeed);
		}
		else {
			// === Tower must of been destroyed, add on the previous speed
			m_pFrozenPlayer->PController->SetMovementSpeed(m_pFrozenPlayer->PController->GetMovementSpeed() + m_pFrozenPlayer->PrevSpeed);
			m_pFrozenPlayer->Animator->SetSpeed(m_pFrozenPlayer->Animator->GetSpeed() + m_pFrozenPlayer->PrevAnimSpeed);
		}
		m_pFrozenPlayer->PController->PauseAnimations(false);
		m_pFrozenPlayer->PController->SetIgnoreInput(false);

		m_pFrozenPlayer->Entity->RemoveFromTracking(this);
		m_pFrozenPlayer->Entity->TakeDamage(DamageInfo(m_fDamage));

		m_pFrozenPlayer->PController = nullptr;
	}

	Inferno::PostAudioEventAtObjectPosition(WWiseIDs::FROZEN_EFFECT, this->gameObject);

	Inferno::Destroy(this->gameObject);
}
// ================== //

// === Collision Events === //
void FreezeBlastWave::OnTriggerEnter(Collider* _collider)
{
	if (_collider->IsTrigger()) {
		return;
	}

	Entity* pEntity = _collider->gameObject->GetComponent<Entity>();
	if (pEntity == nullptr) {
		return;
	}

	if (pEntity->GetTeam() == m_eTeam) {
		return;
	}

	Animator* animator = _collider->gameObject->GetComponentInChildren<Animator>();

	// === Is it a Minion?
	NavAgent* nAgent = _collider->gameObject->GetComponent<NavAgent>();
	if (nAgent != nullptr) {
		FrozenAI newFrozenAI;
		newFrozenAI.Entity = static_cast<AIEntity*>(pEntity);
		// === Is it a Boss?
		if (newFrozenAI.Entity->GetType() == MinionType::BOSS) {
			return;
		}

		newFrozenAI.NAgent = nAgent;
		newFrozenAI.Animator = animator;
		newFrozenAI.PrevSpeed = nAgent->GetSpeed();
		newFrozenAI.PrevAnimSpeed = animator->GetSpeed();

		m_vFrozenMinions.push_back(newFrozenAI);

		nAgent->SetSpeed(0.0f);
		animator->SetSpeed(0.0f);
		newFrozenAI.Entity->SetState(State::STATIONARY);

		pEntity->AddForTracking(this, BindEntityOnReleaseFunction(&FreezeBlastWave::OnEntityRelease, this));
	}

	// === Is it a Player?
	PlayerController* pController = _collider->gameObject->GetComponent<PlayerController>();
	if (pController != nullptr) {
		m_pFrozenPlayer->Entity = pEntity;
		m_pFrozenPlayer->PController = pController;
		m_pFrozenPlayer->Animator = animator;
		m_pFrozenPlayer->PrevSpeed = pController->GetMovementSpeed();
		m_pFrozenPlayer->PrevAnimSpeed = animator->GetSpeed();

		pController->SetMovementSpeed(0.0f);
		pController->SetIgnoreInput(true);
		pController->PauseAnimations(true);
		animator->SetSpeed(0.0f);

		pEntity->AddForTracking(this, BindEntityOnReleaseFunction(&FreezeBlastWave::OnEntityRelease, this));
	}
}
// ======================== //

// ===== Mutators ===== //
void FreezeBlastWave::SetDamage(float _damage)
{
	m_fDamage = _damage;
}

void FreezeBlastWave::SetScales(float _startingScale, float _endingScale)
{
	m_fStartScale = _startingScale;
	m_fEndScale = _endingScale;
}

void FreezeBlastWave::SetTeam(Team _team)
{
	m_eTeam = _team;
}
// ==================== //