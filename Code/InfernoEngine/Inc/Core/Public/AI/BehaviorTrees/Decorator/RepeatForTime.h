///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Repeats the child behavior for the specified amount of time.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DecoratorBehavior.h"

class RepeatForTime : public DecoratorBehavior
{
private:
	float m_fElapsedTime;
	const float m_fTargetTime;
	bool m_bReset;

public:
	RepeatForTime(float _time, BaseBehavior* _child, bool _resetOnInit = true, std::string& _name = std::string());
	~RepeatForTime();

	void Initialize() override;
	void Destroy() override;
	Status Update() override;
};