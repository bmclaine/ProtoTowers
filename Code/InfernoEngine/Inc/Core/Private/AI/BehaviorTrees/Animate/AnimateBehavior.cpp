///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: TimerBehavior is a child of DecoratorBehavior.
//              Returns RUNNING while the timer is not at its target time.
//              Once at its target time, it calls its childs Tick().
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Animate/AnimateBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\Animator.h"
#include "GameObject.h"

AnimateBehavior::AnimateBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: BaseBehavior(_name)
	, m_pEntity(_entity)
	, m_fAnimSpeed(_animSpeed)
{ }

AnimateBehavior::~AnimateBehavior()
{ }

Status AnimateBehavior::Update()
{
	Animator* animator = m_pEntity->gameObject->GetComponentInChildren<Animator>();
	if (animator != nullptr)
	{
		if (animator->GetSpeed() >= 1.0f)
			animator->SetSpeed(m_fAnimSpeed);
	}

	return Status::BT_SUCCESS;
}
