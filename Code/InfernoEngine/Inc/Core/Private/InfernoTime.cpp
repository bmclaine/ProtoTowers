#include "InfernoTime.h"
#include "InfernoTimer.h"
#include "GlobalIncludes.h"

Registry<InfernoTimer> Time::m_sRegistery(20);

LARGE_INTEGER Time::s_liCurrentTime;
LARGE_INTEGER Time::s_liPreviousTime;
LARGE_INTEGER Time::s_liElapsedTime;
LARGE_INTEGER Time::s_liFrequency;

float Time::s_fDeltaTime = 0.0f;
float Time::s_fTrueDeltaTime = 0.0f;
float Time::s_fFixedAccumulator = 0.0f;
float Time::TimeScale = 1.0f;
bool Time::s_bFixedUpdate = false;

#if defined(DEBUG) || defined(_DEBUG)
float Time::s_fFixedDeltaTime = 1.0f / 60.0f;
float Time::s_fFixedUpdateTime = 1.0f / 60.0f;
bool Time::s_bUseUncappedDelta = false;
#else
float Time::s_fFixedDeltaTime = 0.0f;
float Time::s_fFixedUpdateTime = 1.0f / 60.0f;
#endif

const float& Time::DeltaTime = Time::s_fDeltaTime;
const float& Time::TrueDeltaTime = Time::s_fTrueDeltaTime;
const float& Time::FixedDeltaTime = Time::s_fFixedDeltaTime;

bool Time::Initialize()
{
	QueryPerformanceFrequency(&s_liFrequency);
	Reset();

	return true;
}
bool Time::Update()
{
	s_bFixedUpdate = false;

	QueryPerformanceCounter(&s_liCurrentTime);

	s_liElapsedTime.QuadPart = s_liCurrentTime.QuadPart - s_liPreviousTime.QuadPart;
	s_liElapsedTime.QuadPart *= 1000000;
	s_liElapsedTime.QuadPart /= s_liFrequency.QuadPart;
	s_liPreviousTime = s_liCurrentTime;

	s_fTrueDeltaTime = s_fDeltaTime = s_liElapsedTime.QuadPart * 1e-6f;

#if defined(DEBUG) || defined(_DEBUG)
	// While in debug, this forces our timesteps to be capped at a maximum of 1/60th of a second. This will
	// assist in debugging as it will allow for moving the game ahead only a frame at a time with ease.
	if (s_fDeltaTime > s_fFixedDeltaTime && !s_bUseUncappedDelta)
		s_fDeltaTime = s_fFixedDeltaTime;
#endif

	s_fFixedAccumulator += s_fDeltaTime;
	s_fDeltaTime *= TimeScale;

#if defined(DEBUG) || defined(_DEBUG)
	if (!s_bUseUncappedDelta)
	{
		if (s_fFixedAccumulator >= s_fFixedDeltaTime)
		{
			s_fFixedAccumulator = 0;
			s_bFixedUpdate = true;
		}
	}
	else
	{
		if (s_fFixedAccumulator >= s_fFixedUpdateTime)
		{
			s_fFixedDeltaTime = s_fFixedAccumulator * TimeScale;
			s_fFixedAccumulator = 0;
			s_bFixedUpdate = true;
		}
	}
#else
	// Update our fixed delta time if enough time has passed
	if (s_fFixedAccumulator >= s_fFixedUpdateTime)
	{
		s_fFixedDeltaTime = s_fFixedAccumulator * TimeScale;
		s_fFixedAccumulator = 0;
		s_bFixedUpdate = true;
	}
#endif

	const std::vector<InfernoTimer*>& timers = m_sRegistery.GetRegisteredObjects();
	for (size_t i = 0; i < timers.size(); i++)
		timers[i]->Update();

	return true;
}
void Time::Reset()
{
	QueryPerformanceCounter(&s_liCurrentTime);
	s_liPreviousTime = s_liCurrentTime;
}

void Time::RegisterTimer(InfernoTimer* timer)
{
	m_sRegistery.RegisterObject(timer);
}
void Time::UnregisterTimer(InfernoTimer* timer)
{
	m_sRegistery.UnregisterObject(timer);
}