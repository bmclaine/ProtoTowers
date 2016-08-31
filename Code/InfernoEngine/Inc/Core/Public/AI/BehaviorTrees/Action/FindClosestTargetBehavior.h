///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This will call the entities function to find its closest target, if any.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"

class AIEntity;

class FindClosestTargetBehavior : public ActionBehavior
{
private:
	AIEntity *m_pEntity;

public:
	FindClosestTargetBehavior(AIEntity* _entity, std::string& _name = std::string());
	~FindClosestTargetBehavior();

	Status Update() override;
	void Destroy() override;
};