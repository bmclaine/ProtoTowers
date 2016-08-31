///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will ONLY PLAY AN ATTACKING ANIMATION.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Animate\AnimateBehavior.h"

class AttackAnimationBehavior : public AnimateBehavior
{
public:
	AttackAnimationBehavior(AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string());
	~AttackAnimationBehavior();

	void Destroy() override;
	Status Update() override;
};