///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Sets the entitys animation to IDLE.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Animate\IdleAnimationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

IdleAnimationBehavior::IdleAnimationBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: AnimateBehavior(_entity, _animSpeed, _name)
{ }

IdleAnimationBehavior::~IdleAnimationBehavior()
{ }

void IdleAnimationBehavior::Destroy()
{ }

Status IdleAnimationBehavior::Update()
{
	/// Sets the speed of the animation
	AnimateBehavior::Update();

	m_pEntity->SetAnimation(AnimationType::IDLE);
	m_pEntity->UpdateAnimationConditions();

	return Status::BT_SUCCESS;
}

