///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: Returns SUCCESS if either of its children return TRUE/SUCCESS.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Condition/ORBehavior.h"

ORBehavior::ORBehavior(BaseBehavior* _child1, BaseBehavior* _child2, std::string& _name)
	: ConditionBehavior(_name)
	, m_pChild1(_child1)
	, m_pChild2(_child2)
{ }

ORBehavior::~ORBehavior()
{
	Destroy();
}

void ORBehavior::Destroy()
{
	if (m_pChild1 != nullptr)
	{
		m_pChild1->Destroy();
		delete m_pChild1;
		m_pChild1 = nullptr;
	}
	if (m_pChild2 != nullptr)
	{
		m_pChild2->Destroy();
		delete m_pChild2;
		m_pChild2 = nullptr;
	}
}

Status ORBehavior::Update()
{
	if (m_pChild1->Tick() == Status::BT_SUCCESS || m_pChild2->Tick() == Status::BT_SUCCESS)
		return Status::BT_SUCCESS;
	return Status::BT_FAILURE;
}
