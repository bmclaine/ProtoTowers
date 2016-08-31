#include "stdafx.h"
#include "PoisonTurret.h"

#include "PoisonAOEProjectile.h"
#include "PoisonSprayProjectile.h"
#include "TurretProjectile.h"

// === Define 
#define SECONDARY_SHOTTIME 3.0f

// === Constructor / Destructor === //
PoisonTurret::PoisonTurret(GameObject* _gameObject, Transform* _transform) : BaseTurret(_gameObject, _transform)
{
	m_pSecondaryProjectileTimer = nullptr;
}

PoisonTurret::~PoisonTurret()
{
	SAFE_DELETE(m_pSecondaryProjectileTimer);
}
// ================================ //

// === Interface === //
void PoisonTurret::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	BaseTurret::PostInit(objectMap, dataMap);

	if (m_sProjectileID == "PoisonAOEProjectile") {
		m_bAOEProjectile = true;
	}
}

void PoisonTurret::OnEnable()
{
	BaseTurret::OnEnable();

	if (m_uiUpgradeLevel == 2 && m_pSecondaryProjectileTimer == nullptr) {
		m_pSecondaryProjectileTimer = new InfernoTimer();
		m_pSecondaryProjectileTimer->SetTime(SECONDARY_SHOTTIME);
	}
}
// ================= //

// === Protected Interface === //
IComponent& PoisonTurret::operator=(IComponent& _rhs)
{
	PoisonTurret* rhsPT = dynamic_cast<PoisonTurret*>(&_rhs);
	if (rhsPT == nullptr) {
		return *this;
	}

	return operator=(*rhsPT);
}

PoisonTurret& PoisonTurret::operator=(const PoisonTurret& _rhs)
{
	if (this != &_rhs) {
		m_bAOEProjectile = _rhs.m_bAOEProjectile;

		BaseTurret::operator=(_rhs);
	}

	return *this;
}

IComponent* PoisonTurret::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PoisonTurret* newPT = new PoisonTurret(_gameObject, _transform);
	*newPT = *this;

	return newPT;
}

void PoisonTurret::Shoot()
{
	if (m_pFireTimer->IsFinished()) {
		m_pFireTimer->Reset();

		GameObject* projectile = Inferno::Instantiate(m_sProjectileID);
		projectile->transform->SetPosition(m_pProjectileSpawn->GetPosition());
		projectile->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));

		TurretProjectile* tProj = projectile->GetComponent<TurretProjectile>();
		tProj->m_f3Direction = m_pGunTransform->WorldForward();
		tProj->SetDamage(m_fDamage);
		tProj->SetRange(m_fRange);

		if (m_pSecondaryProjectileTimer != nullptr && m_pSecondaryProjectileTimer->IsFinished()) {
			m_pSecondaryProjectileTimer->SetTime(SECONDARY_SHOTTIME);
			m_pSecondaryProjectileTimer->Reset();

			// === Shoot the Secondary Shot
			GameObject* projectile = Inferno::Instantiate("PoisonAOEProjectile");
			projectile->transform->SetPosition(m_pProjectileSpawn->GetPosition());
			projectile->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));

			TurretProjectile* tProj = projectile->GetComponent<TurretProjectile>();
			tProj->m_f3Direction = m_pGunTransform->WorldForward();
			tProj->SetDamage(m_fDamage);
			tProj->SetRange(m_fRange);

			Vector3 toTarget = m_pTarget->transform->GetPosition() - transform->GetPosition();
			float distance = toTarget.Length();
			if (distance > 4.5f) {
				toTarget = (m_pTarget->transform->GetPosition() + (m_pTarget->transform->Forward() * 3.5)) - transform->GetPosition();
				distance = toTarget.Length();
			}
			tProj->SetRange(distance);
		}

		Inferno::PostAudioEventAtObjectPosition(POISON_TURRET_ATTACK_2, projectile);
	}
}
// =========================== //