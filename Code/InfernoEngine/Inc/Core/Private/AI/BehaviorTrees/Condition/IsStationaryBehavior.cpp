///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BaseBehavior is the base class of ALL behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../Public/AI/BehaviorTrees/Condition/IsStationaryBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

IsStationaryBehavior::IsStationaryBehavior(AIEntity* _entity, std::string& _name)
	: ConditionBehavior(_name)
	, m_pEntity(_entity)
{ }

IsStationaryBehavior::~IsStationaryBehavior()
{ }

void IsStationaryBehavior::Destroy()
{ }

Status IsStationaryBehavior::Update()
{
	if (m_pEntity->GetState() == State::STATIONARY)
		return Status::BT_FAILURE;
	return Status::BT_SUCCESS;
}

