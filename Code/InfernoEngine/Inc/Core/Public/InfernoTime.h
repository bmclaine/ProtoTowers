#pragma once

#include "Registry.h"

#include <Windows.h>

class InfernoTimer;

class Time
{
private:
	static Registry<InfernoTimer> m_sRegistery;

	static LARGE_INTEGER s_liCurrentTime;
	static LARGE_INTEGER s_liPreviousTime;
	static LARGE_INTEGER s_liElapsedTime;
	static LARGE_INTEGER s_liFrequency;
	static float s_fDeltaTime;
	static float s_fTrueDeltaTime;
	static float s_fFixedDeltaTime;
	static float s_fFixedAccumulator;
	static float s_fFixedUpdateTime;
#if defined(DEBUG) || defined(_DEBUG)
	static bool s_bUseUncappedDelta;
#endif
	static bool s_bFixedUpdate;

	static bool Initialize();
	static bool Update();
	static void Reset();

	static void RegisterTimer(InfernoTimer* timer);
	static void UnregisterTimer(InfernoTimer* timer);

public:
	static const float& DeltaTime;
	// === TrueDeltaTime
	//  Represents the time between frames, without the TimeScale taken into account for
	static const float& TrueDeltaTime;
	static const float& FixedDeltaTime;
	static float TimeScale;

	friend class InfernoEngine;
	friend class InfernoTimer;
};

