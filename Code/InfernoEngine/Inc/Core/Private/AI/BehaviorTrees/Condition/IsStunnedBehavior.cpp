///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: If the entity is stunned, plays the stun animation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Condition\IsStunnedBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

IsStunnedBehavior::IsStunnedBehavior(AIEntity* _entity, std::string& _name)
	: ConditionBehavior(_name)
	, m_pEntity(_entity)
{ }

IsStunnedBehavior::~IsStunnedBehavior()
{ }

void IsStunnedBehavior::Destroy()
{ }

Status IsStunnedBehavior::Update()
{
	if (m_pEntity->GetState() == State::STUNNED)
	{
		m_pEntity->SetAnimation(AnimationType::IDLE);
		m_pEntity->UpdateAnimationConditions();
		// Return failure to improve the flow of the sequence that it'll be on
		return Status::BT_FAILURE;
	}
	return Status::BT_SUCCESS;
}
