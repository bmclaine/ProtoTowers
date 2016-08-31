#include "stdafx.h"
#include "MortarTower.h"

#include "MortarProjectile.h"

// ===== Constructor / Destructor ===== //
MortarTower::MortarTower(GameObject* _gameObject, Transform* _transform) : BaseTurret(_gameObject, _transform)
{
	m_pMortar = nullptr;
}

MortarTower::~MortarTower()
{

}
// ==================================== //

// ===== Interface ===== //
void MortarTower::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{
	BaseTurret::PostInit(_objectMap, _dataMap);

	ObjectData* data = _dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;

	propName = "MinRange";
	prop = data->GetProperty(propName);
	SetMinRange((float)atof(prop->value.c_str()));
}

void MortarTower::OnEnable()
{
	if (m_pMortar == nullptr) {
		m_pMortar = this->gameObject->GetComponentInChildren<MeshRenderer>()->gameObject;
	}

	BaseTurret::OnEnable();
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& MortarTower::operator=(IComponent& _rhs)
{
	MortarTower* rhsMT = dynamic_cast<MortarTower*>(&_rhs);
	if (rhsMT == nullptr) {
		return *this;
	}

	return operator=(*rhsMT);
}

MortarTower& MortarTower::operator=(const MortarTower& _rhs)
{
	if (this != &_rhs) {
		m_fMinRange = _rhs.m_fMinRange;

		BaseTurret::operator=(_rhs);
	}

	return *this;
}

IComponent* MortarTower::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	MortarTower* newMT = new MortarTower(_gameObject, _transform);
	*newMT = *this;

	return newMT;
}

void MortarTower::Shoot()
{
	if (m_pFireTimer->IsFinished()) {
		m_pFireTimer->Reset();

		// === Is the Target too close?
		float enemyDistance = (m_pTarget->transform->GetPosition() - transform->GetPosition()).LengthSq();
		if (enemyDistance < m_fMinRange) {
			m_pTarget = nullptr;
			return;
		}

		// === Spawn the Projectile
		GameObject* projectile = Inferno::Instantiate(m_sProjectileID);
		projectile->transform->SetPosition(m_pProjectileSpawn->GetPosition());
		projectile->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));

		// === Set the Values
		MortarProjectile* mProj = projectile->GetComponent<MortarProjectile>();
		mProj->SetDamage(m_fDamage);
		mProj->SetTargetDistance(PredictiveTargetting(m_pTarget->transform->GetPosition()));
		mProj->SetDirection(m_pMortar->transform->WorldForward());

		Inferno::PostAudioEventAtObjectPosition(MORTAR_TURRET_ATTACK, projectile);
	}
}

void MortarTower::FindTarget()
{
	Entity* closestEnemy = nullptr;
	Vector3 toEnemy;
	float shortestDistanceSqrd = FLT_MAX, currDistanceSqrd = FLT_MAX;

	unsigned int count = (unsigned int)m_EnemiesInRange.size();
	for (unsigned int i = 0; i < count; ++i) {
		toEnemy = m_EnemiesInRange[i]->transform->GetPosition() - transform->GetPosition();
		currDistanceSqrd = toEnemy.LengthSq();

		if (currDistanceSqrd < shortestDistanceSqrd && currDistanceSqrd > m_fMinRange) {
			shortestDistanceSqrd = currDistanceSqrd;
			closestEnemy = m_EnemiesInRange[i];
		}
	}

	m_pTarget = closestEnemy;
}

bool MortarTower::RotateTowardsTarget()
{
	if (m_pGunTransform == nullptr)
		return false;

	Vector3 toTarget = m_pTarget->transform->GetPosition() - m_pMortar->transform->GetPosition();
	toTarget.Normalize();

	float dotResult = (toTarget * m_pMortar->transform->WorldForward());
	if (dotResult < 0.9985f) {
		if ((toTarget * m_pMortar->transform->WorldRight()) > 0) {
			m_pMortar->transform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));
		}
		else {
			m_pMortar->transform->Rotate(Vector3(0.0f, -m_fTurnRate * Time::DeltaTime, 0.0f));
		}

		if (dotResult > 0.94f)
			return true;
		return false;
	}

	return true;
}

void MortarTower::RotateTowardsStartingDirection()
{
	if (!m_pTargetResetTimer->IsFinished()) {
		return;
	}

	float currRot = m_pMortar->transform->GetRotation().y;
	// === Make sure the Rotation is in the right magnitude
	float pi = ToRadians(180.0f);
	while (currRot > pi * 2) {
		currRot -= pi * 2;
	}
	while (currRot < 0.0f) {
		currRot += pi * 2;
	}

	if (abs(m_fStartingYRot - currRot) > 0.1f) {
		if (m_fStartingYRot > currRot) {
			if (m_fStartingYRot - currRot <= pi) {
				m_pMortar->transform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));
			}
			else {
				m_pMortar->transform->Rotate(Vector3(0.0f, -m_fTurnRate * Time::DeltaTime, 0.0f));
			}
		}
		else {
			if (currRot - m_fStartingYRot <= pi) {
				m_pMortar->transform->Rotate(Vector3(0.0f, -m_fTurnRate * Time::DeltaTime, 0.0f));
			}
			else {
				m_pMortar->transform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));
			}
		}
	}
}
// =============================== //

// ===== Private Interface ===== //
float MortarTower::PredictiveTargetting(Vector3 _targetPos)
{
	Vector3 toTarget = _targetPos - m_pProjectileSpawn->GetPosition();
	float targetDistance = toTarget.Magnitude();

	if (targetDistance <= 5.0f) {
		return targetDistance - 0.5f;
	}
	else if (targetDistance <= 10.0f) {
		return targetDistance - 1.0f;
	}
	else {
		return targetDistance - 2.0f;
	}
}
// ============================= //

// ===== Mutators ===== //
void MortarTower::SetMinRange(float _minRange)
{
	m_fMinRange = _minRange * _minRange;
}
// ==================== //