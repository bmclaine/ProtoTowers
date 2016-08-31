///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Plays the Treants Jump animation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AnimateBehavior.h"

class AIEntity;

class JumpAnimationBehavior : public AnimateBehavior
{
private:
	bool  m_bPlayedSound;

public:
	JumpAnimationBehavior(AIEntity* _entity, float _animSpeed = 1.0f, std::string& _name = std::string());
	~JumpAnimationBehavior();

	void Destroy() override;
	void Initialize() override;
	Status Update() override;
};