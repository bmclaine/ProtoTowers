///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: MoveToBehavior will move an entity to a specified location.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Action\ActionBehavior.h"

class NavAgent;

class MoveToBehavior : public virtual ActionBehavior
{
private:
	NavAgent* m_pAgent;

public:
	MoveToBehavior(NavAgent* _agent, std::string& _name = std::string());
	~MoveToBehavior( void );
	void Destroy( void ) override;

	Status Update( void ) override;
};

