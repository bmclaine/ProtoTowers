///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Sets a NavAgents path flag to true.
//              A new path will be calculated on the next frame, by AIModule.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Action\ActionBehavior.h"
#include <string>

class NavAgent;

class RequestPathBehavior : public ActionBehavior
{
private:
	NavAgent* m_pAgent;

public:
	RequestPathBehavior( NavAgent* _agent, std::string& _name = std::string() );
	virtual ~RequestPathBehavior( void );

	Status Update( void ) override;
	void Destroy( void ) override;
};