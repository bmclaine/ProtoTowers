///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Charge at target until you're within range OR a distance has been met.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ActionBehavior.h"
#include "MathLib.h"

class NavAgent;
class Transform;
class AIEntity;

class ChargeAtTargetBehavior : public ActionBehavior
{
private:
	NavAgent *m_pAgent;

public:
	ChargeAtTargetBehavior(NavAgent* _agent, std::string& _name = std::string());
	~ChargeAtTargetBehavior();

	void Destroy() override;
	Status Update() override; 
};