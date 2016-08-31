///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: CompositeBehavior derives from BaseBehavior.
//              It's a base class of any behavior that requires multiple children.
//              (i.e. Sequence, Selector, etc.)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// Engine Includes
#include "../Public/AI/BehaviorTrees/Composite/CompositeBehavior.h"


// Defaults
CompositeBehavior::CompositeBehavior(std::string& _name)
	: BaseBehavior(_name)
{ }

CompositeBehavior::~CompositeBehavior()
{
	ClearChildren();
}

// Interface
void CompositeBehavior::AddChild(BaseBehavior* _child)
{
	m_vChildren.push_back(_child);
}

void CompositeBehavior::RemoveChild(BaseBehavior* _child)
{
	unsigned int size = (unsigned int)m_vChildren.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vChildren[i] == _child)
		{
			m_vChildren.erase(m_vChildren.begin() + i);
			return;
		}
	}
}

void CompositeBehavior::ClearChildren()
{
	unsigned int size = (unsigned int)m_vChildren.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		m_vChildren[i]->Destroy();
		delete m_vChildren[i];
	}
	m_vChildren.clear();
}

BehaviorType CompositeBehavior::GetType() const
{
	return BehaviorType::COMPOSITE;
}

