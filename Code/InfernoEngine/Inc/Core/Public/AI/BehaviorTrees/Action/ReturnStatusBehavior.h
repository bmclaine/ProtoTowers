///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns whatever Status the user wants upon initialization.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"

class ReturnStatusBehavior : public ActionBehavior
{
private:
	Status m_eStatusToReturn;

public:
	ReturnStatusBehavior(Status _statusToReturn, std::string& _name = std::string());
	~ReturnStatusBehavior();

	void Destroy() override;
	Status Update() override;
};