///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Makes an AIEntity turn to its target. Not error checked - MUST HAVE A TARGET.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"

class NavAgent;
class AIEntity;
class Transform;

class TurnToTarget : public ActionBehavior
{
private:
	NavAgent* m_pAgent;
	AIEntity* m_pEntity;

public:
	TurnToTarget(AIEntity* _entity, NavAgent* _agent, std::string& _name = std::string());
	~TurnToTarget();

	void Destroy() override;
	Status Update() override;
};