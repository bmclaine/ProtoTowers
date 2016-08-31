#include "stdafx.h"
#include "PoisonAOEProjectile.h"

#include "PoisonSprayArea.h"

// === Defines
#define PI 3.141592f
#define STARTING_ARCH 4.712388f
#define HALF_PI 1.570796f
#define ARCH_BUFFER 0.1f
#define POISONAREA_DESTROYTIME 2.5f

// === Constructor / Destructor === //
PoisonAOEProjectile::PoisonAOEProjectile(GameObject* _gameObject, Transform* _transform) : TurretProjectile(_gameObject, _transform)
{

}

PoisonAOEProjectile::~PoisonAOEProjectile()
{

}
// ================================ //

// === Interface === //
void PoisonAOEProjectile::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	TurretProjectile::PostInit(objectMap, dataMap);
}

void PoisonAOEProjectile::OnEnable()
{
	m_v3StartingPosition = transform->GetPosition();
}

void PoisonAOEProjectile::Update()
{
	Vector3 position = transform->GetPosition();
	float distTravelled = abs((m_v3StartingPosition - Vector3(position.x, m_v3StartingPosition.y, position.z)).Magnitude());
	float archRatio = (distTravelled / (m_fRange - 1.0f));

	float result = sin((archRatio * HALF_PI) + STARTING_ARCH);
	result = -(result + 1.0f);
	float yOffset = (result / 1) * position.y;
	Vector3 velocity = (m_f3Direction + Vector3(0.0f, yOffset, 0.0f)) * m_fSpeed * Time::DeltaTime;
	transform->SetPosition(transform->GetPosition() + velocity);

	if (archRatio >= 1.0f - ARCH_BUFFER) {
		GameObject* pArea = Inferno::Instantiate("PoisonArea");
		pArea->transform->SetPosition(transform->GetPosition());
		pArea->transform->SetYPosition(0.0f);
		PoisonSprayArea* pSprayArea = pArea->GetComponent<PoisonSprayArea>();
		pSprayArea->SetEnemyTeam(GetEnemyTeam());
		pSprayArea->SetDamage(m_pDamageInfo->IncomingDamage);
		Inferno::Destroy(pArea, POISONAREA_DESTROYTIME);
		Inferno::Destroy(this->gameObject);
	}
}
// ================= //

// === Protected Interface === //
IComponent& PoisonAOEProjectile::operator=(IComponent& _rhs)
{
	PoisonAOEProjectile* rhsPAOE = dynamic_cast<PoisonAOEProjectile*>(&_rhs);
	if (rhsPAOE == nullptr) {
		return *this;
	}

	return operator=(*rhsPAOE);
}

PoisonAOEProjectile& PoisonAOEProjectile::operator=(const PoisonAOEProjectile& _rhs)
{
	if (this != &_rhs) {
		TurretProjectile::operator=(_rhs);
	}

	return *this;
}

IComponent* PoisonAOEProjectile::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PoisonAOEProjectile* newPAOE = new PoisonAOEProjectile(_gameObject, _transform);
	*newPAOE = *this;

	return newPAOE;
}
// =========================== //

// === Private Interface === //
Team PoisonAOEProjectile::GetEnemyTeam() const
{
	if (gameObject->GetLayer() == (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ) {
		return Team::BLUE;
	}
	else {
		return Team::RED;
	}
}
// ========================= //

// === Collision Events === //
void PoisonAOEProjectile::OnTriggerEnter(Collider* _collider)
{

}
// ======================== //