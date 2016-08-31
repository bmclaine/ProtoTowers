///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Plays the SPECIAL animation of the TANK ONLY!
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Animate/SpecialAnimationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

SpecialAnimationBehavior::SpecialAnimationBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: AnimateBehavior(_entity, _animSpeed, _name)
{ }

SpecialAnimationBehavior::~SpecialAnimationBehavior()
{ }

void SpecialAnimationBehavior::Destroy()
{ }

Status SpecialAnimationBehavior::Update()
{
	/// Sets the speed of the animation
	AnimateBehavior::Update();

	m_pEntity->SetAnimation(AnimationType::SPECIAL);
	m_pEntity->UpdateAnimationConditions();

	return Status::BT_SUCCESS;
}

