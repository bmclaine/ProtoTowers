#include "stdafx.h"
#include "BaseTurret.h"

#include "Components\MeshRenderer.h"	// <-- TEMP (Probably)
#include "EventSystem.h"
#include "Physics\SphereCollider.h"
#include "TurretProjectile.h"
#include "ModuleInterface.h"

#include <limits.h>
#include <unordered_map>

// === Defines
#define TARGETRESET_TIME 2.0f

// ===== Constructor / Destructor ===== //
BaseTurret::BaseTurret(GameObject* _gameObject, Transform* _tranform) : BaseTower(_gameObject, _tranform)
{
	m_pFireTimer = new InfernoTimer();
	m_pTargetResetTimer = new InfernoTimer();
	m_pGunTransform = nullptr;
	m_pTarget = nullptr;
	m_pProjectileSpawn = nullptr;

	m_bNoUpdate = false;
}

BaseTurret::~BaseTurret()
{
	delete m_pFireTimer;
	delete m_pTargetResetTimer;
}
// ==================================== //

// === Interface === //
void BaseTurret::Init()
{
	// === Post Creation Setup (More than likely Temp)
	if (transform->GetChildCount() > 0) {
		if (m_pGunTransform == nullptr)
			m_pGunTransform = transform->GetChild(0);
		if (m_pProjectileSpawn == nullptr)
			m_pProjectileSpawn = transform->GetChild(0)->GetChild(0);
	}
}

void BaseTurret::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	BaseTower::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "GunTransform";
	Property* prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0)
		m_pGunTransform = (Transform*)objectMap.find((unsigned int)ID)->second;

	propName = "ProjectileSpawn";
	prop = data->GetProperty(propName);
	ID = atoi(prop->value.c_str());
	if (ID > 0)
		m_pProjectileSpawn = (Transform*)objectMap.find((unsigned int)ID)->second;

	propName = "FireRate";
	prop = data->GetProperty(propName);
	SetFireRate((float)atof(prop->value.c_str()));

	propName = "Range";
	prop = data->GetProperty(propName);
	m_fRange = (float)atof(prop->value.c_str());

	propName = "TurnRate";
	prop = data->GetProperty(propName);
	SetTurnRate((float)atof(prop->value.c_str()));

	propName = "Damage";
	prop = data->GetProperty(propName);
	SetDamage((float)atof(prop->value.c_str()));

	propName = "ProjectileID";
	prop = data->GetProperty(propName);
	SetProjectile(prop->value);
}

void BaseTurret::OnEnable()
{
	if (m_pGunTransform != nullptr) {
		m_pGunTransform->Rotate(Vector3(0.0f, m_fStartingYRot, 0.0f));
		BaseTower::OnEnable();
	}

	SetRange(m_fRange);

	m_EnemiesInRange.clear();

	EventSystem::GetInstance()->RegisterForEvent("CutSceneActive", this, BindEventFunction(&BaseTurret::OnStopUpdating, this));
}

void BaseTurret::OnDisable()
{
	EventSystem::GetInstance()->UnregisterFromEvent("CutSceneActive", this, BindEventFunction(&BaseTurret::OnStopUpdating, this));

	BaseTower::OnDisable();
}

void BaseTurret::Update()
{
	if (m_bNoUpdate) {
		return;
	}

	if (m_pTarget == nullptr && m_EnemiesInRange.size() > 0) {
		FindTarget();
		m_pTargetResetTimer->SetTime(0.0f);
	}
	else if (m_EnemiesInRange.size() == 0 && m_pTargetResetTimer->GetSetTimeSeconds() == 0.0f) {
		m_pTargetResetTimer->SetTime(TARGETRESET_TIME);
		m_pTargetResetTimer->Reset();
		OnNoTargets();
	}

	if (m_pTarget != nullptr) {
		if (RotateTowardsTarget()) {
			if (m_uiUpgradeLevel == 2 && m_eType == TowerTypes::TT_AUTOTURRET) {
				DoubleShot();
			}
			else {
				Shoot();
			}
		}
	}
	else {
		RotateTowardsStartingDirection();
	}

	// Change the color of the turret, based on how much damage it's taken
	if (m_pRenderer != nullptr)
	{
		float damageTaken = m_fMaxHealth - m_fHealth;
		float multiplier = 2.0f;
		float damageToColor = (255.0f - (damageTaken * multiplier)) / 255.0f;
		m_pRenderer->SetColor(Color(1, damageToColor, damageToColor, 1));
	}
}

void BaseTurret::TakeDamage(DamageInfo _damage)
{
	// TODO - Add Hit Effect

	BaseTower::TakeDamage(_damage);
}

void BaseTurret::Death()
{
	BaseTower::Death();
}
// ================= //

// === Collision Events === //
void BaseTurret::OnTriggerEnter(Collider* _collider)
{
	BaseTower::OnTriggerEnter(_collider);

	if (_collider->IsTrigger() || _collider->gameObject == gameObject)
		return;

	Entity* enemy = _collider->gameObject->GetComponent<Entity>();

	if (enemy != nullptr && enemy->GetTeam() == GetEnemyTeam()) {
		if (!m_EnemiesInRange.contains(enemy)) {
			m_EnemiesInRange.add(enemy);
			enemy->AddForTracking(this->gameObject, BindEntityOnReleaseFunction(&BaseTurret::OnEntityRelease, this));
		}
	}
}

void BaseTurret::OnTriggerExit(Collider* _collider)
{
	if (_collider->IsTrigger() || _collider->gameObject == gameObject)
		return;

	Entity* enemy = _collider->gameObject->GetComponent<Entity>();

	if (enemy != nullptr && enemy->GetTeam() == GetEnemyTeam()) {
		if (enemy == m_pTarget) {
			m_pTarget = nullptr;
		}

		m_EnemiesInRange.remove(enemy);
		enemy->RemoveFromTracking(this->gameObject);
	}
}
// ======================== //

// === Protected Interface === //
IComponent& BaseTurret::operator=(IComponent& _rhs)
{
	BaseTurret* rhsBT = dynamic_cast<BaseTurret*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

BaseTurret& BaseTurret::operator=(const BaseTurret& _rhs)
{
	if (this != &_rhs) {
		m_sProjectileID = _rhs.m_sProjectileID;
		m_fRange = _rhs.m_fRange;
		m_fTurnRate = _rhs.m_fTurnRate;
		m_fFireRate = _rhs.m_fFireRate;
		m_fDamage = _rhs.m_fDamage;
		m_pFireTimer->SetTime(m_fFireRate);
		m_pTarget = nullptr;
		m_EnemiesInRange.clear();

		BaseTower::operator=(_rhs);
	}

	return *this;
}

IComponent* BaseTurret::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BaseTurret* newBaseTurret = new BaseTurret(_gameObject, _transform);
	*newBaseTurret = *this;

	return newBaseTurret;
}

void BaseTurret::Shoot()
{
	if (m_pFireTimer->IsFinished()) {
		m_pFireTimer->Reset();

		GameObject* projectile = Inferno::Instantiate(m_sProjectileID);
		projectile->transform->SetPosition(m_pProjectileSpawn->GetPosition());
		projectile->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));
		// === TEMP HACK (Until Transform rotation is fixed)
		TurretProjectile* tProj = projectile->GetComponent<TurretProjectile>();
		tProj->m_f3Direction = m_pGunTransform->WorldForward();
		tProj->SetDamage(m_fDamage);
		tProj->SetRange(m_fRange * transform->GetScale().x);

		Inferno::PostAudioEventAtObjectPosition(AUTO_TURRET_ATTACK, projectile);
	}
}

void BaseTurret::FindTarget()
{
	Entity* closestEnemy = nullptr;
	Vector3 toEnemy;
	float shortestDistanceSqrd = FLT_MAX, currDistanceSqrd = FLT_MAX;

	unsigned int count = (unsigned int)m_EnemiesInRange.size();
	for (unsigned int i = 0; i < count; ++i) {
		toEnemy = m_EnemiesInRange[i]->transform->GetPosition() - transform->GetPosition();
		currDistanceSqrd = toEnemy.LengthSq();

		if (currDistanceSqrd < shortestDistanceSqrd) {
			shortestDistanceSqrd = currDistanceSqrd;
			closestEnemy = m_EnemiesInRange[i];
		}
	}

	m_pTarget = closestEnemy;
}

bool BaseTurret::RotateTowardsTarget()
{
	if (m_pGunTransform == nullptr)
		return false;

	Vector3 toTarget = m_pTarget->transform->GetPosition() - m_pGunTransform->GetPosition();
	toTarget.Normalize();

	float dotResult = (toTarget * m_pGunTransform->WorldForward());
	if (dotResult < 0.9985f) {
		if ((toTarget * m_pGunTransform->WorldRight()) > 0) {
			m_pGunTransform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));
		}
		else {
			m_pGunTransform->Rotate(Vector3(0.0f, -m_fTurnRate * Time::DeltaTime, 0.0f));
		}

		if (dotResult > 0.94f)
			return true;
		return false;
	}

	return true;
}

void BaseTurret::RotateTowardsStartingDirection()
{
	if (!m_pTargetResetTimer->IsFinished()) {
		return;
	}

	float currRot = m_pGunTransform->GetRotation().y;
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
				m_pGunTransform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));
			}
			else {
				m_pGunTransform->Rotate(Vector3(0.0f, -m_fTurnRate * Time::DeltaTime, 0.0f));
			}
		}
		else {
			if (currRot - m_fStartingYRot <= pi) {
				m_pGunTransform->Rotate(Vector3(0.0f, -m_fTurnRate * Time::DeltaTime, 0.0f));
			}
			else {
				m_pGunTransform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));
			}
		}
	}
}

void BaseTurret::OnEntityRelease(Entity* _releasedEntity)
{
	// === Was the released entity our Target?
	if (m_pTarget == _releasedEntity) {
		m_pTarget = nullptr;
	}

	m_EnemiesInRange.remove(_releasedEntity);
}

void BaseTurret::OnNoTargets()
{

}
// =========================== //

// === Private Interface === //
void BaseTurret::DoubleShot()
{
	if (m_pFireTimer->IsFinished()) {
		m_pFireTimer->Reset();

		// === 1st Projectile
		GameObject* projectile = Inferno::Instantiate(m_sProjectileID);
		projectile->transform->SetPosition(m_pProjectileSpawn->GetPosition() + (m_pProjectileSpawn->WorldRight() * -0.5));
		projectile->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));
		
		TurretProjectile* tProj = projectile->GetComponent<TurretProjectile>();
		tProj->m_f3Direction = m_pGunTransform->WorldForward();
		tProj->SetDamage(m_fDamage);
		tProj->SetRange(m_fRange * transform->GetScale().x);

		// === 2nd Projectile
		projectile = Inferno::Instantiate(m_sProjectileID);
		projectile->transform->SetPosition(m_pProjectileSpawn->GetPosition() + (m_pProjectileSpawn->WorldRight() * 0.5));
		projectile->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));
		
		tProj = projectile->GetComponent<TurretProjectile>();
		tProj->m_f3Direction = m_pGunTransform->WorldForward();
		tProj->SetDamage(m_fDamage);
		tProj->SetRange(m_fRange * transform->GetScale().x);

		Inferno::PostAudioEventAtObjectPosition(AUTO_TURRET_ATTACK, projectile);
	}
}
// ========================= //

// ===== Events ===== //
void BaseTurret::OnStopUpdating(void* _pData)
{
	m_bNoUpdate = *(static_cast<bool*>(_pData));
}
// ================== //

// ===== Accessors ===== //
Entity* BaseTurret::GetTarget() const
{
	return m_pTarget;
}

Team BaseTurret::GetEnemyTeam() const
{
	if (m_eTeam == Team::BLUE) {
		return Team::RED;
	}
	else {
		return Team::BLUE;
	}
}

float BaseTurret::GetFireRate() const
{
	return m_fFireRate;
}

float BaseTurret::GetRange() const
{
	return m_fRange;
}

float BaseTurret::GetTurnRate() const
{
	return ToDegrees(m_fTurnRate);
}
// ===================== //

// ===== Mutators ===== //
void BaseTurret::SetGun(Transform* _gunTransform)
{
	m_pGunTransform = _gunTransform;
}

void BaseTurret::SetProjectileSpawn(Transform* _projSpawn)
{
	m_pProjectileSpawn = _projSpawn;
}

void BaseTurret::SetProjectile(std::string _projectileID)
{
	m_sProjectileID = _projectileID;
}

void BaseTurret::SetFireRate(float _roundsPerSecond)
{
	m_fFireRate = 1 / _roundsPerSecond;
	m_pFireTimer->SetTime(m_fFireRate);
}

void BaseTurret::SetRange(float _radius)
{
	SphereCollider* rangeCollider = gameObject->GetComponent<SphereCollider>();
	if (rangeCollider == nullptr) {
		rangeCollider = gameObject->AddComponent<SphereCollider>();
		rangeCollider->SetIsTrigger(true);
	}

	float scaledRadius = _radius / transform->GetScale().x;

	rangeCollider->SetRadius(scaledRadius);
	m_fRange = scaledRadius;
}

void BaseTurret::SetTurnRate(float _anglesPerSecond)
{
	m_fTurnRate = ToRadians(_anglesPerSecond);
}

void BaseTurret::SetDamage(float _damage)
{
	m_fDamage = _damage;
}
// ==================== //