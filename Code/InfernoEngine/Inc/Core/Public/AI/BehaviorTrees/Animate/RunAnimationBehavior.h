///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: RunAnimationBehavior will ONLY PLAY A RUNNING ANIMATION.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Animate\AnimateBehavior.h"

class AIEntity;

class RunAnimationBehavior : public virtual AnimateBehavior
{
public:
	RunAnimationBehavior( AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string() );
	virtual ~RunAnimationBehavior( void );

	void Destroy( void ) override;
	Status Update( void ) override;
};

