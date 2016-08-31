///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: CompositeBehavior derives from BaseBehavior.
//              It's a base class of any behavior that requires multiple children.
//              (i.e. Sequence, Selector, etc.)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// Engine Includes
#include "../Public/AI/BehaviorTrees/BaseBehavior.h"

// STL Includes
#include <vector>

class CompositeBehavior : public virtual BaseBehavior
{
protected:
	typedef std::vector<BaseBehavior*> Behaviors;
	Behaviors m_vChildren;

public:
	// Defaults
	CompositeBehavior( std::string& _name = std::string() );
	virtual ~CompositeBehavior(void);

	// Interface
	void AddChild(BaseBehavior* _child);
	void RemoveChild(BaseBehavior* _child);
	void ClearChildren( void );

	// Accessors
	BehaviorType GetType( void ) const override;
};
