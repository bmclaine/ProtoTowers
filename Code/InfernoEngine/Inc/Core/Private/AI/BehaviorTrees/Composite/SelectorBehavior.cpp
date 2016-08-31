///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Selector inherits from CompositeBehavior.
//              Will process each child until one of them returns true.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Composite\SelectorBehavior.h"
#include "AssertHelper.h"


SelectorBehavior::SelectorBehavior(std::string& _name)
	: CompositeBehavior(_name)
{ }

SelectorBehavior::~SelectorBehavior()
{ }

void SelectorBehavior::Initialize()
{
	m_itCurrentChild = m_vChildren.begin();
	ASSERT((*m_itCurrentChild) != nullptr, "SequenceBehavior::Initialize() -> Sequence doesn't have any children");
}

Status SelectorBehavior::Update()
{
	Status status = Status::BT_INVALID;
	ASSERT((*m_itCurrentChild) != nullptr, "SequenceBehavior::Initialize() -> Sequence doesn't have any children");

	for (;;)
	{
		status = (*m_itCurrentChild)->Tick();
		if (status != Status::BT_FAILURE)
			return status;
		else
			++m_itCurrentChild;
		if (m_itCurrentChild == m_vChildren.end())
			return status;
	}
	// If we've made it this far, something's wrong
	return status;
}

void SelectorBehavior::Destroy()
{ }

