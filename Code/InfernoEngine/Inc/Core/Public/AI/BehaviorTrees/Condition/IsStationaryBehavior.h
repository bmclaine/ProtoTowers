///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BaseBehavior is the base class of ALL behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class AIEntity;

class IsStationaryBehavior : public ConditionBehavior
{
private:
	AIEntity* m_pEntity;

public:
	IsStationaryBehavior(AIEntity* _entity, std::string& _name = std::string());
	~IsStationaryBehavior();

	void Destroy() override;
	Status Update() override;
};