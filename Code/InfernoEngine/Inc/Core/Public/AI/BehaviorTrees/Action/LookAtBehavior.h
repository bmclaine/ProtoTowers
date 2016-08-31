///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Makes an entity look at its target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"

class AIEntity;
class Transform;
class NavAgent;

class LookAtBehavior : public ActionBehavior
{
private:
	AIEntity* m_pEntity;
	NavAgent* m_pAgent;

public:
	LookAtBehavior(AIEntity* _entity, NavAgent* _agent, std::string& _name = std::string());
	~LookAtBehavior();

	void Destroy() override;
	Status Update() override;
};
