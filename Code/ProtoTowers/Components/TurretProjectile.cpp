#include "stdafx.h"
#include "TurretProjectile.h"

#include "../Core/iDamagable.h"

// === TEMP (Until Physics layers are set up
#include "MiniMinion.h"

// ===== Constructor / Destructor ===== //
TurretProjectile::TurretProjectile(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_fTravelledDist = 0.0f;
	m_pDamageInfo = new DamageInfo(0.0f);
	m_bDestroyed = true;
}

TurretProjectile::~TurretProjectile()
{
	delete m_pDamageInfo;
}
// ==================================== //

// ===== Interface ===== //
void TurretProjectile::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "PlayerMultiplier";
	Property* prop = data->GetProperty(propName);
	m_pDamageInfo->PlayerMultiplier = (float)atof(prop->value.c_str());

	propName = "TowerMultiplier";
	prop = data->GetProperty(propName);
	m_pDamageInfo->TowerMultiplier = (float)atof(prop->value.c_str());

	propName = "MinionMultiplier";
	prop = data->GetProperty(propName);
	m_pDamageInfo->MinionMultiplier = (float)atof(prop->value.c_str());

	propName = "IncomingDamage";
	prop = data->GetProperty(propName);
	m_pDamageInfo->IncomingDamage = (float)atof(prop->value.c_str());

	propName = "Speed";
	prop = data->GetProperty(propName);
	m_fSpeed = (float)atof(prop->value.c_str());

	propName = "Range";
	prop = data->GetProperty(propName);
	m_fRange = (float)atof(prop->value.c_str());
}

void TurretProjectile::OnEnable()
{
	m_fTravelledDist = 0.0f;
	m_bDestroyed = false;
}

void TurretProjectile::OnDisable()
{
	m_bDestroyed = true;
}

void TurretProjectile::Update()
{
	if (m_bDestroyed) {
		return;
	}

	Vector3 velocity = /*transform->Forward()*/ m_f3Direction * (m_fSpeed * Time::DeltaTime); 
	m_fTravelledDist += m_fSpeed * Time::DeltaTime;

	transform->Translate(velocity);

	if (m_fTravelledDist >= m_fRange) {
		Inferno::Destroy(this->gameObject);
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& TurretProjectile::operator=(IComponent& _rhs)
{
	TurretProjectile* rhsTP = dynamic_cast<TurretProjectile*>(&_rhs);
	if (rhsTP == nullptr) {
		return *this;
	}

	return operator=(*rhsTP);
}

TurretProjectile& TurretProjectile::operator=(const TurretProjectile& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fRange = _rhs.m_fRange;
		m_fSpeed = _rhs.m_fSpeed;
		m_fTravelledDist = _rhs.m_fTravelledDist;
		m_bDestroyed = _rhs.m_bDestroyed;
		*m_pDamageInfo = *_rhs.m_pDamageInfo;
	}

	return *this;
}

IComponent* TurretProjectile::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	TurretProjectile* newCopy = new TurretProjectile(gameObject, transform);
	*newCopy = *this;

	return newCopy;
}

unsigned int TurretProjectile::GetEnemyLayer()
{
	if (gameObject->GetLayer() == (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ) {
		return (unsigned int)CollisionLayers::LAYER_FRIENDLY;
	}
	else {
		return (unsigned int)CollisionLayers::LAYER_ENEMY;
	}
}
// =============================== //

// ===== Collision Events ===== //
void TurretProjectile::OnTriggerEnter(Collider* _collider)
{
	if (!_collider->IsTrigger() && _collider->gameObject->GetName() == "AutoTurret") {
		int debug = 0;
	}

	if (!m_bDestroyed && !_collider->IsTrigger()) {
		if (_collider->gameObject->GetLayer() == GetEnemyLayer()) {
			iDamagable* hitObject = _collider->gameObject->GetComponent<iDamagable>();

			if (hitObject != nullptr) {
				hitObject->TakeDamage(*m_pDamageInfo);
			}
		}
		else {
			AIEntity* hitObject = _collider->gameObject->GetComponent<AIEntity>();

			if (hitObject != nullptr) {
				return;
			}
		}

		Inferno::Destroy(this->gameObject);
		m_fTravelledDist = 0.0f;
		m_fSpeed = 0.0f;
		m_bDestroyed = true;
	}
}
// ============================ //

// ===== Accessors ===== //

// === GetDamageInfo
//  IN : void
//  OUT : DamageInfo* - Pointer to the DamageInfo structure
//
//  Returns a pointer to the DamageInfo Structure this Projectile is using.
DamageInfo* TurretProjectile::GetDamageInfo() const
{
	return m_pDamageInfo;
}

// === GetDamage
//  IN : void
//  OUT : float - The Damage value
//
//  Returns the Damage value from the DamageInfo structure this projectile is using.
float TurretProjectile::GetDamage() const
{
	return m_pDamageInfo->IncomingDamage;
}

// === GetSpeed
//  IN : void
//  OUT : float - The Speed value of the projectile
//
//  Returns the Speed value of the projectile, which is in Units per Second.
float TurretProjectile::GetSpeed() const
{
	return m_fSpeed;
}

// === GetRange
//  IN : void
//  OUT : The Range value of the projectile
//
//  Returns the Range value of the projectile, which is how far the projectile can travel before destroying itself.
float TurretProjectile::GetRange() const
{
	return m_fRange;
}
// ===================== //

// ===== Mutators ===== //

// === SetDamage
//  IN : DamageInfo - The DamageInfo structure to copy from
//  OUT : void
//
//  Sets the values of this projectile's DamageInfo to match the one that is passed in.
void TurretProjectile::SetDamage(DamageInfo _damInfo)
{
	*m_pDamageInfo = _damInfo;
}

// === SetDamage
//  IN : float - Damage value
//       float - (Optional) Minion multiplier value
//       float - (Optional) Player multiplier value
//       float - (Optional) Tower multipler value
//  OUT : void
//
//  Sets the individual parts of this projectiles DamageInfo structure.
void TurretProjectile::SetDamage(float _damage, float _minionMulti, float _playerMulti, float _towerMulti)
{
	m_pDamageInfo->IncomingDamage = _damage;
	m_pDamageInfo->MinionMultiplier = _minionMulti;
	m_pDamageInfo->PlayerMultiplier = _playerMulti;
	m_pDamageInfo->TowerMultiplier = _towerMulti;
}

// === SetSpeed
//  IN : float - Speed value
//  OUT : void
// 
//  Sets the projectile's speed value, using a Unit's per Second method.
void TurretProjectile::SetSpeed(float _speed)
{
	m_fSpeed = _speed;
}

// === SetRange
//  IN : float - The Range value
//  OUT : void
//
//  Sets how far the projectile can travel, in Units, before it destroys itself.
void TurretProjectile::SetRange(float _range)
{
	m_fRange = _range;
}
// ==================== //