#include "stdafx.h"
#include "PoisonSprayProjectile.h"

#include "PoisonEffect.h"

// ===== Constructor / Destructor ===== //
PoisonSprayProjectile::PoisonSprayProjectile(GameObject* _gameObject, Transform* _transform) : TurretProjectile(_gameObject, _transform)
{

}

PoisonSprayProjectile::~PoisonSprayProjectile()
{

}
// ==================================== //

// ===== Interface ===== //
void PoisonSprayProjectile::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	TurretProjectile::PostInit(objectMap, dataMap);
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& PoisonSprayProjectile::operator=(IComponent& _rhs)
{
	PoisonSprayProjectile* rhsPSP = dynamic_cast<PoisonSprayProjectile*>(&_rhs);
	if (rhsPSP == nullptr) {
		return *this;
	}

	return operator=(*rhsPSP);
}

PoisonSprayProjectile& PoisonSprayProjectile::operator=(const PoisonSprayProjectile& _rhs)
{
	if (this != &_rhs) {
		TurretProjectile::operator=(_rhs);
	}

	return *this;
}

IComponent* PoisonSprayProjectile::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PoisonSprayProjectile* newPSP = new PoisonSprayProjectile(_gameObject, _transform);
	*newPSP = *this;

	return newPSP;
}
// =============================== //

// ===== Collision Events ===== //
void PoisonSprayProjectile::OnTriggerEnter(Collider* _collider)
{
	if (!_collider->IsTrigger()) {
		if (_collider->gameObject->GetLayer() == GetEnemyLayer()) {
			Entity* hitObject = _collider->gameObject->GetComponent<Entity>();

			if (hitObject != nullptr && hitObject->GetHealth() > 0.0f) {
				// === Does the Entity already have a PoisonEffect on it?
				PoisonEffect* pEffect = hitObject->gameObject->GetComponentInChildren<PoisonEffect>();
				if (pEffect == nullptr) {
					// === Add on the PoisonEffect
					GameObject* pEffectObject = Inferno::Instantiate("PoisonEffect");
					pEffectObject->transform->SetParent(hitObject->transform);
					pEffectObject->transform->SetPosition(0.0f, 0.0f, 0.0f);
					pEffect = pEffectObject->GetComponent<PoisonEffect>();
					pEffect->SetDamage(m_pDamageInfo->IncomingDamage);
					pEffect->StartEffectTimer();
				}
				else {
					// === Reset the PoisonEffect
					pEffect->ResetEffectTimer();
				}
			}
		}
	}
}
// ============================ //