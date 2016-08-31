///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Checks if the BOSS hasn't made much progress while trying to move.
//              Checks if the DISTANCE has been met in the amount of TIME.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Condition/IsBossStuckBehavior.h"
#include "../../../../ProtoTowers/Components/BossMinion.h"
#include "InfernoTime.h"

IsBossStuckBehavior::IsBossStuckBehavior(BossMinion* _boss, float _time, float _distance, std::string& _name)
	: ConditionBehavior(_name)
	, m_pBoss(_boss)
	, m_fTargetTime(_time)
	, m_fAccumulatedTime(0.0f)
	, m_fTargetDistance(_distance)
	, m_fAccumulatedDistance(0.0f)
{ }

IsBossStuckBehavior::~IsBossStuckBehavior()
{ }

void IsBossStuckBehavior::Initialize()
{
	m_fAccumulatedDistance = m_fAccumulatedTime = 0.0f;
}

void IsBossStuckBehavior::Destroy()
{ }

Status IsBossStuckBehavior::Update()
{
	/// Update both accumulations
	m_fAccumulatedTime += Time::DeltaTime;
	m_fAccumulatedDistance += m_pBoss->GetDistanceTraveled();

	if (m_fAccumulatedTime > m_fTargetTime && m_fAccumulatedDistance < m_fTargetDistance)
		return Status::BT_SUCCESS;
	return Status::BT_FAILURE;
}

