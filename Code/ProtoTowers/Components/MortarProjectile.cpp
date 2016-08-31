#include "stdafx.h"
#include "MortarProjectile.h"

#include "ModuleInterface.h"

#include <math.h>
#include <vector>

// === Defines
#define ARCH 3.141592f * 2.0f
#define ARCH_BUFFER 0.1f

// ===== Constructor / Destructor ===== //
MortarProjectile::MortarProjectile(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	// === TEMP
	m_fTargetDistance = 20.0f;
}

MortarProjectile::~MortarProjectile()
{

}
// ==================================== //

// ===== Interface ===== //
void MortarProjectile::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{
	ObjectData* data = _dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;

	propName = "Damage";
	prop = data->GetProperty(propName);
	SetDamage((float)atof(prop->value.c_str()));

	propName = "Radius";
	prop = data->GetProperty(propName);
	m_fRadius = (float)atof(prop->value.c_str());

	propName = "Speed";
	prop = data->GetProperty(propName);
	SetSpeed((float)atof(prop->value.c_str()));
}

void MortarProjectile::OnEnable()
{
	// === Capture the Startng Position, for Arch tracking
	m_v3StartingPosition = transform->GetPosition();
}

void MortarProjectile::Update()
{
	Vector3 position = transform->GetPosition();
	float distTravelled = abs((m_v3StartingPosition - Vector3(position.x, m_v3StartingPosition.y, position.z)).Magnitude());
	float archRatio = (distTravelled / m_fTargetDistance) + ARCH_BUFFER;

	Vector3 velocity = (m_v3Direction + Vector3(0.0f, sin(archRatio * ARCH), 0.0f)) * m_fSpeed * Time::DeltaTime;
	transform->SetPosition(transform->GetPosition() + velocity);

	if (archRatio >= 1.0f - ARCH_BUFFER) {
		Explode();
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& MortarProjectile::operator=(IComponent& _rhs)
{
	MortarProjectile* rhsMP = dynamic_cast<MortarProjectile*>(&_rhs);
	if (rhsMP == nullptr) {
		return *this;
	}

	return operator=(*rhsMP);
}

MortarProjectile& MortarProjectile::operator=(const MortarProjectile& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fSpeed = _rhs.m_fSpeed;
		m_fTargetDistance = _rhs.m_fTargetDistance;
		m_fDamage = _rhs.m_fDamage;
		m_fRadius = _rhs.m_fRadius;
	}

	return *this;
}

IComponent* MortarProjectile::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	MortarProjectile* newMP = new MortarProjectile(_gameObject, _transform);
	*newMP = *this;

	return newMP;
}

unsigned int MortarProjectile::GetEnemyLayer()
{
	if (gameObject->GetLayer() == (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ) {
		return (unsigned int)CollisionLayers::LAYER_FRIENDLY;
	}
	else {
		return (unsigned int)CollisionLayers::LAYER_ENEMY;
	}
}

void MortarProjectile::Explode()
{
	std::vector<RayCastHit> hitInfo;
	if (InfernoEngine_Physics::SphereOverlap(transform->GetPosition(), m_fRadius, hitInfo)) {
		Entity* hitEnemy = nullptr;
		size_t count = hitInfo.size();
		for (size_t i = 0; i < count; ++i) {
			hitEnemy = hitInfo[i].collider->gameObject->GetComponent<Entity>();
			if (hitEnemy != nullptr && hitEnemy->gameObject->GetLayer() == GetEnemyLayer()) {
				hitEnemy->TakeDamage(DamageInfo(m_fDamage));
			}
		}
	}

	// === Audio
	Inferno::PostAudioEventAtObjectPosition(MORTAR_TURRET_PROJECTILE_HIT, this->gameObject);

	// === Explosion Effect
	GameObject* explosion = Inferno::Instantiate("Mortar Explosion");
	Vector3 pos = transform->GetPosition();
	explosion->transform->SetPosition(pos);
	Inferno::Destroy(explosion, 1.5f);

	Inferno::Destroy(this->gameObject);
}
// =============================== //

// ===== Accessors ===== //
float MortarProjectile::GetSpeed() const
{
	return m_fSpeed;
}

float MortarProjectile::GetTargetDistance() const
{
	return m_fTargetDistance;
}

float MortarProjectile::GetDamage() const
{
	return m_fDamage;
}
// ===================== //

// ===== Mutators ===== //
void MortarProjectile::SetDirection(Vector3 _direction)
{
	m_v3Direction = _direction;
}

void MortarProjectile::SetSpeed(float _speed)
{
	m_fSpeed = _speed;
}

void MortarProjectile::SetTargetDistance(float _distance)
{
	m_fTargetDistance = _distance;
}

void MortarProjectile::SetDamage(float _damage)
{
	m_fDamage = _damage;
}

void MortarProjectile::SetRadius(float _radius)
{
	m_fRadius = _radius;
}
// ==================== //
