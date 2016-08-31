///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will move the attached NavAgent along its path,
//					which has already been calculated to find its waypoint.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Action\ActionBehavior.h"

class NavAgent;

class MoveToWaypointBehavior : public virtual ActionBehavior
{
private:
	NavAgent* m_pAgent;

public:
	MoveToWaypointBehavior( NavAgent* _agent, std::string& _name = std::string() );
	virtual ~MoveToWaypointBehavior();

	void Destroy( void ) override;
	Status Update( void ) override;
};

