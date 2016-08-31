///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: RunAnimationBehavior will ONLY PLAY A RUNNING ANIMATION.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Animate\RunAnimationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"

RunAnimationBehavior::RunAnimationBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: AnimateBehavior(_entity, _animSpeed, _name)
{ }

RunAnimationBehavior::~RunAnimationBehavior()
{ }

Status RunAnimationBehavior::Update()
{
	/// Sets the speed of the animation
	AnimateBehavior::Update();

	m_pEntity->SetAnimation(AnimationType::RUNNING);
	m_pEntity->UpdateAnimationConditions();

	return Status::BT_SUCCESS;
}

void RunAnimationBehavior::Destroy()
{ }

