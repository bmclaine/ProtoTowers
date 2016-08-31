///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS if the entity is looking in the direction of its target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class AIEntity;
class NavAgent;

class IsLookingAt : public ConditionBehavior
{
private:
	AIEntity* m_pEntity;
	NavAgent* m_pAgent;

public:
	IsLookingAt(AIEntity* _entity, NavAgent* _agent, std::string& _name = std::string());
	~IsLookingAt();

	void Destroy() override;
	Status Update() override;
};