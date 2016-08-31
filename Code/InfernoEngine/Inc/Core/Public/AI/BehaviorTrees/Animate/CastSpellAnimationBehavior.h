///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Plays the CastSpell animation of the boss minion.
//              Returns true when the animation is completely finished.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AnimateBehavior.h"

class AIEntity;

class CastSpellAnimationBehavior : public AnimateBehavior
{
private:
	float m_fAnimationTime;

public:
	CastSpellAnimationBehavior(AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string());
	~CastSpellAnimationBehavior();

	void Initialize() override;
	void Destroy() override;
	Status Update() override;
};