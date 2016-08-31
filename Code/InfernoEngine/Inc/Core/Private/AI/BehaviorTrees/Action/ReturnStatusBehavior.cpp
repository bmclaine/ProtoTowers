///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns whatever Status the user wants upon initialization.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/ReturnStatusBehavior.h"

ReturnStatusBehavior::ReturnStatusBehavior(Status _statusToReturn, std::string& _name)
	: ActionBehavior(_name)
	, m_eStatusToReturn(_statusToReturn)
{ }

ReturnStatusBehavior::~ReturnStatusBehavior()
{ }

void ReturnStatusBehavior::Destroy()
{ }

Status ReturnStatusBehavior::Update()
{
	return m_eStatusToReturn;
}

