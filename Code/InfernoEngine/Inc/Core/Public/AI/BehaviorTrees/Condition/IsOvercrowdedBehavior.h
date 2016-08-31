///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS if the threshold is met. FAILURE otherwise.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class BossMinion;
class GameObject;

class IsOverCrowdedBehavior : public ConditionBehavior
{
private:
	BossMinion* m_pEntity;
	unsigned int m_uiNearbyThreshold;

public:
	IsOverCrowdedBehavior(BossMinion* _entity, unsigned int _nearbyThreshold, std::string& _name = std::string());
	~IsOverCrowdedBehavior();

	void Destroy() override;
	Status Update() override;
};