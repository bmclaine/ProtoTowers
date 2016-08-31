///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Plays the Treants Jump animation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Animate/JumpAnimationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Components\Animator.h"
#include "../../Interface/Public/Engine.h"
#include "InfernoTime.h"
#include "GameObject.h"

#include "../../Interface/Public/ModuleInterface.h"

JumpAnimationBehavior::JumpAnimationBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: AnimateBehavior(_entity, _animSpeed, _name)
	, m_bPlayedSound(false)
{ }

JumpAnimationBehavior::~JumpAnimationBehavior()
{ }

void JumpAnimationBehavior::Initialize()
{
	AnimateBehavior::Initialize();
	m_bPlayedSound = false;
}

void JumpAnimationBehavior::Destroy()
{ }

Status JumpAnimationBehavior::Update()
{
	/// Sets the speed of the animation
	AnimateBehavior::Update();

	m_pEntity->SetAnimation(AnimationType::JUMP);
	m_pEntity->UpdateAnimationConditions();

	/// Make sure to only play this once
	if (m_bPlayedSound == false)
	{
		m_bPlayedSound = true;
		Inferno::PostAudioEvent(416769816U);
	}

	return Status::BT_SUCCESS;

//	m_fAnimationTime -= Time::DeltaTime;
//	if (m_fAnimationTime < 0.0f)
//		return Status::BT_SUCCESS;
//	return Status::BT_RUNNING;
}

