///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BehaviorTree is the class that will be the base of everything.
//              It will have one BaseBehavior child that will act as the Tree's root.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Action\ActionBehavior.h"

class NavAgent;

class MoveToTargetBehavior : public ActionBehavior
{
private:
	NavAgent* m_pAgent;

public:
	MoveToTargetBehavior(NavAgent* _agent, std::string& _name = std::string());
	~MoveToTargetBehavior();

	void Destroy() override;
	Status Update() override;
};