///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS if the entity has a target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class AIEntity;

class HasTargetBehavior : public ConditionBehavior
{
private:
	AIEntity* m_pEntity;

public:
	HasTargetBehavior(AIEntity* _entity, std::string& _name = std::string());
	~HasTargetBehavior();

	void Destroy() override;
	Status Update() override;
};