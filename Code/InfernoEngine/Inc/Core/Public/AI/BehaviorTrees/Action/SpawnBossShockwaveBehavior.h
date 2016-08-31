///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Spawns a boss minion shockwave at the boss's location.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"

class AIEntity;

class SpawnBossShockwaveBehavior : public ActionBehavior
{
private:
	AIEntity* m_pEntity;

public:
	SpawnBossShockwaveBehavior(AIEntity* _entity, std::string& _name = std::string());
	~SpawnBossShockwaveBehavior();

	void Destroy() override;
	Status Update() override;
};

