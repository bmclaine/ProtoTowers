#include "stdafx.h"
#include "PoisonSprayArea.h"

#include "PoisonEffect.h"

// === Defines
#define GE_STARTSIZE 0.5f
#define GE_ENDSIZE 3.0f
#define GE_TOTALTIME 1.5f

// ===== Constructor / Destructor ===== //
PoisonSprayArea::PoisonSprayArea(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pGroundEffect = nullptr;
}

PoisonSprayArea::~PoisonSprayArea()
{

}
// ==================================== //

// ===== Interface ===== //
void PoisonSprayArea::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	// === Nothing is needed from the Prefab File
}

void PoisonSprayArea::OnEnable()
{
	// === Get the Ground Effect
	if (m_pGroundEffect == nullptr) {
		m_pGroundEffect = gameObject->GetComponentInChildren<SpriteRenderer>();
	}

	m_pGroundEffect->transform->SetScale(GE_STARTSIZE, GE_STARTSIZE, GE_STARTSIZE);
	m_fElapsedTime = 0.0f;
	IComponent::OnEnable();
}

void PoisonSprayArea::OnDisable()
{
	auto iter = m_lInfectedEntities.begin();
	while (iter != m_lInfectedEntities.end()) {
		iter->pEffect->StartEffectTimer();
		++iter;
	}

	m_lInfectedEntities.clear();
	IComponent::OnDisable();
}

void PoisonSprayArea::Update()
{
	// === Handle the GroundEffect growth
	if (m_fElapsedTime < GE_TOTALTIME) {
		m_fElapsedTime += Time::DeltaTime;
		float newSize = (m_fElapsedTime / GE_TOTALTIME) * (GE_ENDSIZE - GE_STARTSIZE) + GE_STARTSIZE;
		m_pGroundEffect->transform->SetScale(newSize, newSize, newSize);
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& PoisonSprayArea::operator=(IComponent& _rhs)
{
	PoisonSprayArea* rhsPSA = dynamic_cast<PoisonSprayArea*>(&_rhs);
	if (rhsPSA == nullptr) {
		return *this;
	}

	return operator=(*rhsPSA);
}

PoisonSprayArea& PoisonSprayArea::operator=(const PoisonSprayArea& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fDamage = _rhs.m_fDamage;
	}

	return *this;
}

IComponent* PoisonSprayArea::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PoisonSprayArea* newPSA = new PoisonSprayArea(_gameObject, _transform);
	*newPSA = *this;

	return newPSA;
}
// =============================== //

// === Private Interface === //
bool PoisonSprayArea::AlreadyTrackingEntity(Entity* _entity)
{
	auto iter = m_lInfectedEntities.begin();
	while (iter != m_lInfectedEntities.end()) {
		if (iter->pEntity == _entity) {
			return true;
		}
		++iter;
	}

	return false;
}
// ========================= //

// ===== CollisionEvents ===== //
void PoisonSprayArea::OnTriggerEnter(Collider* _collider)
{
	if (!_collider->IsTrigger()) {
		// === Is it an Enemy Entity?
		Entity* enemy = _collider->gameObject->GetComponent<Entity>();
		if (enemy == nullptr || enemy->GetTeam() != m_eEnemyTeam || enemy->GetHealth() == 0.0f) {
			return;
		}

		// === Is this Entity already being tracked by this tower?
		if (AlreadyTrackingEntity(enemy)) {
			return;
		}

		// === Does is it already infected?
		PoisonEffect* pEffect = enemy->gameObject->GetComponentInChildren<PoisonEffect>();
		if (pEffect == nullptr) {
			// === Not Infected, add the effect
			GameObject* pEffectObject = Inferno::Instantiate("PoisonEffect");
			pEffectObject->transform->SetParent(enemy->transform);
			pEffectObject->transform->SetPosition(0.0f, 0.0f, 0.0f);
			pEffect = pEffectObject->GetComponent<PoisonEffect>();
			pEffect->SetDamage(m_fDamage);
//			pEffect->StartEffectTimer();
//			pEffect->StopEffectTimer();
		}
		else {
			// === Infected, make sure to stop the timer
			pEffect->StopEffectTimer();
		}

		// === Track this Infected Entity
		InfectedEntity newInfectedEntity(enemy, pEffect);
		m_lInfectedEntities.push_back(newInfectedEntity);
	}
}

void PoisonSprayArea::OnTriggerExit(Collider* _collider)
{
	if (!_collider->IsTrigger()) {
		// === Is it an Enemy Entity?
		Entity* enemy = _collider->gameObject->GetComponent<Entity>();
		if (enemy == nullptr || enemy->GetTeam() != m_eEnemyTeam) {
			return;
		}
		
		// === Find, Start the Timer, and Remove 
		auto iter = m_lInfectedEntities.begin();
		while (iter != m_lInfectedEntities.end()) {
			if (iter->pEntity == enemy) {
				iter->pEffect->StartEffectTimer();
				m_lInfectedEntities.erase(iter);
				return;
			}
			++iter;
		}
	}
}
// =========================== //

// === Mutators === //
void PoisonSprayArea::SetDamage(float _damage)
{
	m_fDamage = _damage;
}

void PoisonSprayArea::SetEnemyTeam(Team _enemyTeam)
{
	m_eEnemyTeam = _enemyTeam;
}
// ================ //