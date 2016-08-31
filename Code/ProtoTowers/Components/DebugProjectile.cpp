#include "stdafx.h"
#include "DebugProjectile.h"

// ===== Constructor / Destructor ===== //
DebugProjectile::DebugProjectile(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_fSpeed = 10.0f;
}

DebugProjectile::~DebugProjectile()
{

}
// ==================================== //

// ===== Interface ===== //
void DebugProjectile::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void DebugProjectile::Update()
{
	transform->Translate(Vector3(0.0f, 0.0f, m_fSpeed * Time::DeltaTime));
}
// ===================== //

// ===== Collision Events ===== //
void DebugProjectile::OnTriggerEnter(Collider* _collider)
{
	if (!_collider->IsTrigger()) {
		if (_collider->gameObject->GetTag() == "Player") {
			return;
		}

		GameObject* hitObject = _collider->gameObject;
		DebugBreak();
		Inferno::Destroy(this->gameObject);
	}
}
// ============================ //

// === Protected Interface === //
IComponent& DebugProjectile::operator=(IComponent& _rhs)
{
	DebugProjectile* rhsDP = dynamic_cast<DebugProjectile*>(&_rhs);
	if (rhsDP == nullptr) {
		return *this;
	}

	return operator=(*rhsDP);
}

DebugProjectile& DebugProjectile::operator=(const DebugProjectile& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fSpeed = _rhs.m_fSpeed;
	}

	return *this;
}

IComponent* DebugProjectile::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	DebugProjectile* newDebugProj = new DebugProjectile(gameObject, transform);
	*newDebugProj = *this;

	return newDebugProj;
}
// =========================== //