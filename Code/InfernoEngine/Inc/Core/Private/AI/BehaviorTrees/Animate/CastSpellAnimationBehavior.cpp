///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Plays the CastSpell animation of the boss minion
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Animate/CastSpellAnimationBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "Assets\AnimationController.h"
#include "Assets\Animation.h"
#include "InfernoTime.h"

CastSpellAnimationBehavior::CastSpellAnimationBehavior(AIEntity* _entity, float _animSpeed, std::string& _name)
	: AnimateBehavior(_entity, _animSpeed, _name)
{ }

CastSpellAnimationBehavior::~CastSpellAnimationBehavior()
{ }

void CastSpellAnimationBehavior::Destroy()
{ }

void CastSpellAnimationBehavior::Initialize()
{
	Animation* anim = m_pEntity->GetAnimationController()->GetAnimationFromSet(std::string("Treant_Cast"));
	ASSERT(anim != nullptr, "CastSpellAnimationBehavior::Initialize() -> Entity doesn't have an animation");
	m_fAnimationTime = anim->GetDuration();
}

Status CastSpellAnimationBehavior::Update()
{
	m_pEntity->SetAnimation(AnimationType::CAST_SPELL);
	m_pEntity->UpdateAnimationConditions();

	/// Sets the speed of the animation
	AnimateBehavior::Update();

	m_fAnimationTime -= Time::DeltaTime;
	if (m_fAnimationTime < 0.0f)
		return Status::BT_SUCCESS;
	return Status::BT_RUNNING;
}

