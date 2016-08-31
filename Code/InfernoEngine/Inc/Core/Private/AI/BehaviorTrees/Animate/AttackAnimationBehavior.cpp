///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will ONLY PLAY AN ATTACKING ANIMATION.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Animate\AttackAnimationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "InfernoTime.h"
#include "GameObject.h"
#include "Assets\AnimationController.h"

AttackAnimationBehavior::AttackAnimationBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: AnimateBehavior(_entity, _animSpeed, _name)
{ }

AttackAnimationBehavior::~AttackAnimationBehavior()
{ }

void AttackAnimationBehavior::Destroy()
{ }

Status AttackAnimationBehavior::Update()
{
	/// Sets the speed of the animation
	AnimateBehavior::Update();

	m_pEntity->SetAnimation(AnimationType::ATTACKING);
	m_pEntity->UpdateAnimationConditions();

	return Status::BT_SUCCESS;
}
