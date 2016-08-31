///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: TimerBehavior is a child of DecoratorBehavior.
//              Returns RUNNING while the timer is not at its target time.
//              Once at its target time, it calls its childs Tick().
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/BaseBehavior.h"

class AIEntity;
class Animator;
class GameObject;

class AnimateBehavior : public virtual BaseBehavior
{
protected:
	AIEntity* m_pEntity;
	float     m_fAnimSpeed;

public:
	AnimateBehavior( AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string() );
	~AnimateBehavior( void );

	Status Update() override;
};