///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Plays the SPECIAL animation of the TANK ONLY!
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AnimateBehavior.h"

class AIEntity;

class SpecialAnimationBehavior : public AnimateBehavior
{
public:
	SpecialAnimationBehavior(AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string());
	~SpecialAnimationBehavior();

	void Destroy() override;
	Status Update() override;
};