///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: TimerBehavior is a child of DecoratorBehavior.
//              Returns RUNNING while the timer is not at its target time.
//              Once at its target time, it calls its childs Tick().
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/Decorator/DecoratorBehavior.h"
#include "InfernoTimer.h"

class TimerBehavior : public virtual DecoratorBehavior
{
private:
	float m_fElapsedTime; // -> The time that's passed
	const float m_fTargetTime;  // -> The inputed time to be reached
	bool m_bReset;

public:
	TimerBehavior(float _time, bool _resetAfterFinish, BaseBehavior* _child = nullptr, std::string& _name = std::string());
	virtual ~TimerBehavior( void );
	void Destroy( void ) override;

	// Interface
	virtual void Initialize(void) override;
	virtual Status Update(void) override;
};