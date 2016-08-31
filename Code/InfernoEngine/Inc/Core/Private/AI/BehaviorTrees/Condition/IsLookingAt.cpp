///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS if the entity is looking in the direction of its target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Condition/IsLookingAt.h"
#include "Components\NavAgent.h"
#include "Components\Transform.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "MathLib.h"

IsLookingAt::IsLookingAt(AIEntity* _entity, NavAgent* _agent, std::string& _name)
	: ConditionBehavior(_name)
	, m_pEntity(_entity)
	, m_pAgent(_agent)
{ }

IsLookingAt::~IsLookingAt()
{ }

void IsLookingAt::Destroy()
{ }

Status IsLookingAt::Update()
{
	Vector3 toTarget = m_pEntity->GetEnemyToTrack()->transform->GetPosition() - m_pEntity->transform->GetPosition();
	toTarget.Normalize();
	
	float dot = toTarget * m_pEntity->transform->Forward();

	if (dot < 0.707f) /// 45 degree angle
		return Status::BT_SUCCESS;
	return Status::BT_FAILURE;
}

