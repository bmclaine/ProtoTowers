///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: If the AIEntity within the class has a target to track, the destination will be set to it.
//              If the AIEntity does NOT have a target, the destination will be set to the next path position.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Action\ActionBehavior.h"

class AIEntity;

class SetDestinationBehavior : public ActionBehavior
{
private:
	AIEntity* m_pEntity;

public:
	SetDestinationBehavior(AIEntity* _entity, std::string& _name = std::string());
	~SetDestinationBehavior();

	Status Update() override;
	void Destroy() override;
};