///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: ActionBehavior derives from BaseBehavior.
//              Is the base class for all action based behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"

class AIEntity;

class DealDamageToNearbyEnemiesBehavior : public ActionBehavior
{
private:
	AIEntity* m_pEntity;

public:
	DealDamageToNearbyEnemiesBehavior(AIEntity* _entity, std::string& _name = std::string());
	~DealDamageToNearbyEnemiesBehavior();

	void Destroy() override;
	Status Update() override;
};