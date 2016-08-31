///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Checks if the BOSS hasn't made much progress while trying to move.
//              Checks if the DISTANCE has been met in the amount of TIME.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class BossMinion;

class IsBossStuckBehavior : public ConditionBehavior
{
private:
	BossMinion *m_pBoss;
	float       m_fTargetTime;
	float       m_fAccumulatedTime;
	float       m_fTargetDistance;
	float       m_fAccumulatedDistance;

public:
	IsBossStuckBehavior(BossMinion* _boss, float _time, float _distance, std::string& _name = std::string());
	~IsBossStuckBehavior();

	void Initialize() override;
	void Destroy() override;
	Status Update() override;
};

