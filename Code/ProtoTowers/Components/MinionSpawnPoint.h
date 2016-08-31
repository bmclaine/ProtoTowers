#pragma once
#include "Components\IComponent.h"

class Waypoint;

class MinionSpawnPoint : public IComponent
{
private:
	// === Variables
	Waypoint* m_pStartingWaypoint;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MinionSpawnPoint& operator=(const MinionSpawnPoint& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	MinionSpawnPoint(GameObject* _gameObject, Transform* _transform);
	virtual ~MinionSpawnPoint();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	// ================= //

	// === Mutators === //
	void SetStartingWaypoint(Waypoint* _waypoint);
	// ================ //

	// === Accessors === //
	Waypoint* GetStartingWaypoint() const;
	// ================= //
};

