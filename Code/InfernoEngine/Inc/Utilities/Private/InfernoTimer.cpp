#include "InfernoTimer.h"
#include "InfernoTime.h"

InfernoTimer::InfernoTimer(float seconds, bool trueDelta)
{
	m_OnTimerEnd = nullptr;
	m_bFuncCalled = false;
	m_bPaused = false;
	m_bUseTrueDelta = trueDelta;
	SetTime(seconds);
	Reset();

	Time::RegisterTimer(this);
}
InfernoTimer::InfernoTimer(unsigned int milliseconds, bool trueDelta)
{
	m_OnTimerEnd = nullptr;
	m_bFuncCalled = false;
	m_bPaused = false;
	m_bUseTrueDelta = trueDelta;
	SetTime(milliseconds);
	Reset();

	Time::RegisterTimer(this);
}

InfernoTimer::~InfernoTimer()
{
	Time::UnregisterTimer(this);
}

InfernoTimer& InfernoTimer::operator=(const InfernoTimer& rhs)
{
	m_fTargetTime = rhs.m_fTargetTime;

	return *this;
}

void InfernoTimer::Update()
{
	if (m_fTargetTime == 0.0f || m_bPaused)
		return;

	m_fAccumTime += (m_bUseTrueDelta) ? Time::s_fTrueDeltaTime : Time::DeltaTime;

	if (m_OnTimerEnd != nullptr && m_fAccumTime >= m_fTargetTime && !m_bFuncCalled) {
		m_bFuncCalled = true;
		m_OnTimerEnd();
	}
}

// IN: void
// OUT: void
//
// Restarts the accumulated time
void InfernoTimer::Reset()
{
	m_fAccumTime = 0.0f;
	m_bFuncCalled = false;
	m_bPaused = false;
}
// IN: void
// OUT: void
//
// Resets the Accumulated and Target times
void InfernoTimer::FullReset()
{
	m_fAccumTime = 0.0f;
	m_fTargetTime = 0.0f;
	m_bFuncCalled = false;
	m_bPaused = false;
}

void InfernoTimer::SetTime(float seconds)
{
	if (seconds < 0.0f)
		m_fTargetTime = 0.0f;
	else
		m_fTargetTime = seconds;

	m_fAccumTime = 0.0f;
	m_bFuncCalled = false;
	m_bPaused = false;
}
void InfernoTimer::SetTime(unsigned int milliseconds)
{
	m_fTargetTime = milliseconds * 0.001f;
	m_fAccumTime = 0.0f;
	m_bFuncCalled = false;
	m_bPaused = false;
}

void InfernoTimer::SetOnTimerEndFunction(OnTimerEndFunc _func)
{
	m_OnTimerEnd = _func;
}
void InfernoTimer::UseTrueDelta(bool use)
{
	m_bUseTrueDelta = use;
}
void InfernoTimer::PauseTimer(bool pause)
{
	m_bPaused = pause;
}

float InfernoTimer::DifferenceBetweenTheStartTimeAndTimePassedForBrian() const { return TimeRemaining(); }
float InfernoTimer::TimeRemaining() const
{
	float timeDiff = m_fTargetTime - m_fAccumTime;

	if (timeDiff < 0)
		timeDiff = 0;

	return timeDiff;
}
float InfernoTimer::GetSetTimeSeconds() const { return m_fTargetTime; }
unsigned int InfernoTimer::GetSetTimeMilliseconds() const { return unsigned int(m_fTargetTime * 1000); }

bool InfernoTimer::IsFinished() const
{
	return m_fAccumTime >= m_fTargetTime;
}