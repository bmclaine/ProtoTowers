///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: If the entity is stunned, plays the stun animation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class AIEntity;

class IsStunnedBehavior : public ConditionBehavior
{
private:
	AIEntity* m_pEntity;

public:
	IsStunnedBehavior(AIEntity* _entity, std::string& _name = std::string());
	~IsStunnedBehavior();

	void Destroy() override;
	Status Update() override;
};