///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS when the specified amount of distance has been traveled.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Condition/HasTravelledDistanceBehavior.h"
#include "Components\NavAgent.h"
#include "Components\Transform.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

HasTavelledDistanceBehavior::HasTavelledDistanceBehavior(AIEntity* _entity, NavAgent* _agent, float _distance, bool _resetDistOnInit, std::string& _name)
	: ConditionBehavior(_name)
	, m_pEntity(_entity)
	, m_pAgent(_agent)
	, m_fTargetDistance(_distance)
	, m_fElapsedDistance(0.0f)
	, m_bReset(_resetDistOnInit)
{ }

HasTavelledDistanceBehavior::~HasTavelledDistanceBehavior()
{ }

void HasTavelledDistanceBehavior::Initialize()
{
	ConditionBehavior::Initialize();
	if (m_bReset)
		m_fElapsedDistance = 0.0f;

	m_vPrevPosition = m_pAgent->transform->GetPosition();
}

void HasTavelledDistanceBehavior::Destroy()
{ }

Status HasTavelledDistanceBehavior::Update()
{
	/// Distance traveled so far
	m_fElapsedDistance += (m_vPrevPosition - m_pAgent->transform->GetPosition()).LengthSq();
	m_vPrevPosition = m_pAgent->transform->GetPosition();

	if (m_fElapsedDistance > m_fTargetDistance)
		return Status::BT_SUCCESS;
	return Status::BT_RUNNING;
}

