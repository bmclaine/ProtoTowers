///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Selector inherits from CompositeBehavior.
//              Will process each child until one of them returns true.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Composite\CompositeBehavior.h"

class SelectorBehavior : public virtual CompositeBehavior
{
protected:
	Behaviors::iterator m_itCurrentChild;

public:
	SelectorBehavior( std::string& _name = std::string() );
	~SelectorBehavior( void );

	void Destroy() override;
	void Initialize( void ) override;
	Status Update( void ) override;
};

