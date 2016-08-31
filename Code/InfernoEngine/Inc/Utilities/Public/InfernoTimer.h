// Author: Jonathan Gyurkovics
//
// A simple timer class to handle any timing events that may be needed

#pragma once

#include <functional>

// === Defines
#define BindOnTimerEndFunction(Func, Class) { std::bind(Func, Class) }

class InfernoTimer
{
public:
	// === Typedefs
	typedef std::function<void(void)> OnTimerEndFunc;

private:
	OnTimerEndFunc m_OnTimerEnd;
	float m_fAccumTime;
	float m_fTargetTime;
	bool m_bFuncCalled;
	bool m_bUseTrueDelta;
	bool m_bPaused;

	void Update();

public:
	InfernoTimer(float seconds = 0.0f, bool trueDelta = false);
	InfernoTimer(unsigned int milliseconds, bool trueDelta = false);
	~InfernoTimer();

	InfernoTimer& operator=(const InfernoTimer& rhs);

	// IN: void
	// OUT: void
	//
	// Restarts the accumulated time
	void Reset();
	// IN: void
	// OUT: void
	//
	// Resets the Accumulated and Target times
	void FullReset();

	void SetTime(float seconds);
	void SetTime(unsigned int milliseconds);
	void SetOnTimerEndFunction(OnTimerEndFunc _func);
	void UseTrueDelta(bool use);
	void PauseTimer(bool pause);

	float DifferenceBetweenTheStartTimeAndTimePassedForBrian() const;
	float TimeRemaining() const;
	float GetSetTimeSeconds() const;
	unsigned int GetSetTimeMilliseconds() const;

	bool IsFinished() const;

	friend class Time;
};