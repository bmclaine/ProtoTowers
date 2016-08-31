///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS if the threshold is met. FAILURE otherwise.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Condition/IsOvercrowdedBehavior.h"
#include "../../../../ProtoTowers/Components/BossMinion.h"
#include "GameObject.h"

IsOverCrowdedBehavior::IsOverCrowdedBehavior(BossMinion* _entity, unsigned int _nearbyThreshold, std::string& _name)
	: ConditionBehavior(_name)
	, m_pEntity(_entity)
	, m_uiNearbyThreshold(_nearbyThreshold)
{ }

IsOverCrowdedBehavior::~IsOverCrowdedBehavior()
{ }

void IsOverCrowdedBehavior::Destroy()
{ }

Status IsOverCrowdedBehavior::Update()
{
	return m_pEntity->GetNumEnemiesWithinRange() >= m_uiNearbyThreshold ? Status::BT_SUCCESS : Status::BT_FAILURE;
}

