// Author : Jack Gillette
//
// An Interface class for anything that can take Damage.
#pragma once
class Entity;
struct DamageInfo
{
	float PlayerMultiplier, TowerMultiplier, MinionMultiplier;
	float IncomingDamage;
	Entity* OwningObject;

	DamageInfo(float _damage, Entity* _owningEntity = nullptr, float _playerMulti = 1.0f, float _towerMulti = 1.0f, float _minionMulti = 1.0f) {
		IncomingDamage = _damage;
		OwningObject = _owningEntity;
		PlayerMultiplier = _playerMulti;
		TowerMultiplier = _towerMulti;
		MinionMultiplier = _minionMulti;
	}

	DamageInfo& operator=(const DamageInfo& _rhs) {
		if (this != &_rhs) {
			IncomingDamage = _rhs.IncomingDamage;
			OwningObject = _rhs.OwningObject;
			MinionMultiplier = _rhs.MinionMultiplier;
			PlayerMultiplier = _rhs.PlayerMultiplier;
			TowerMultiplier = _rhs.TowerMultiplier;
		}
		return *this;
	}
};

class iDamagable
{
public:
	// === Interface === //
	virtual void TakeDamage(DamageInfo _damageInfo) = 0;
	// ================= //
};