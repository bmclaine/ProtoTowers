///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Will make an entity idle in position.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Decorator\DecoratorBehavior.h"

class NavAgent;

class WaitBehavior : public virtual DecoratorBehavior
{
private:
	float m_fElapsedTime; // -> The time that's passed since creation.
	float m_fTargetTime;  // -> The time that's input by the user.

public:
	WaitBehavior(float _time = 0.0f, std::string& _name = std::string());
	~WaitBehavior( void );

	void Initialize( void ) override;
	Status Update( void ) override;
};