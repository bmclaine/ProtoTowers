///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS if the entity has a target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Condition/HasTargetBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

HasTargetBehavior::HasTargetBehavior(AIEntity* _entity, std::string& _name)
	: ConditionBehavior(_name)
	, m_pEntity(_entity)
{ }

HasTargetBehavior::~HasTargetBehavior()
{ }

void HasTargetBehavior::Destroy()
{ }

Status HasTargetBehavior::Update()
{
	if (m_pEntity->GetEnemyToTrack() != nullptr)
		return Status::BT_SUCCESS;
	return Status::BT_FAILURE;
}

