// Author: Jack Gillette
//
// A shield around the spawners that, when shot, alerts the player that no damage is getting through.
#pragma once
#include "Components\IComponent.h"

class MinionSpawnerShield : public IComponent
{
private:
	// === Variables
	bool m_bCPShield;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MinionSpawnerShield& operator=(const MinionSpawnerShield& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	MinionSpawnerShield(GameObject* _gameObject, Transform* _transform);
	virtual ~MinionSpawnerShield();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	// ======================== //
};