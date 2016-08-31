///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BehaviorTree is the class that will be the base of everything.
//              It will have one BaseBehavior child that will act as the Tree's root.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DecoratorBehavior.h"

class BaseBehavior;

class DoOnceBehavior : public DecoratorBehavior
{
private:
	bool m_bExecuted;

public:
	DoOnceBehavior(BaseBehavior* _child, std::string& _name = std::string());
	~DoOnceBehavior();

	void Destroy() override;
	Status Update() override;
};