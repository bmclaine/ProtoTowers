///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: SequenceBehavior inherits from BaseBehavior.
//              Continues to process children until one returns failure.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Composite/SequenceBehavior.h"
#include "AssertHelper.h"

// Defaults
SequenceBehavior::SequenceBehavior(std::string& _name)
	: CompositeBehavior(_name)
{ }

SequenceBehavior::~SequenceBehavior()
{ }

void SequenceBehavior::Destroy()
{ }

void SequenceBehavior::Initialize()
{
	m_itCurrentChild = m_vChildren.begin();
	ASSERT((*m_itCurrentChild) != nullptr, "SequenceBehavior::Initialize() -> Sequence doesn't have any children");
}

Status SequenceBehavior::Update()
{
	Status status = Status::BT_INVALID;
	ASSERT((*m_itCurrentChild) != nullptr, "SequenceBehavior::Update() -> Sequence doesn't have any children");

	for (;;)
	{
		status = (*m_itCurrentChild)->Tick();

		if (status != Status::BT_SUCCESS)
			return status;
		else
			++m_itCurrentChild;
		if (m_itCurrentChild == m_vChildren.end())
			return status;
	}
	return status;
}

BehaviorType SequenceBehavior::GetType() const
{
	return BehaviorType::SEQUENCE;
}

