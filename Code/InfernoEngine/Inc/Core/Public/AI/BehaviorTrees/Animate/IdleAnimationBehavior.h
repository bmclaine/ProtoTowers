///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Sets the entitys animation to IDLE.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Animate\AnimateBehavior.h"

class AIEntity;

class IdleAnimationBehavior : public AnimateBehavior
{
public:
	IdleAnimationBehavior(AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string());
	~IdleAnimationBehavior();

	void Destroy() override;
	Status Update() override;
};

