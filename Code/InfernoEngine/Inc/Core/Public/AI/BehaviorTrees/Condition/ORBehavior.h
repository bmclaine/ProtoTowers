///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: Returns SUCCESS if either of its children return TRUE/SUCCESS.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"

class ORBehavior : public ConditionBehavior
{
private:
	BaseBehavior* m_pChild1;
	BaseBehavior* m_pChild2;

public:
	ORBehavior(BaseBehavior* _child1, BaseBehavior* _child2, std::string& _name = std::string());
	~ORBehavior();

	void Destroy() override;
	Status Update() override;
};