#pragma once

#include <d3d11.h>

#include "Rendering\Common\StateMonitor.h"
#include "Rendering\Common\StateMonitorArray.h"

class DX11RasterizerStateStageState
{
public:
	StateMonitor<int> m_RasterizerState;
	StateMonitor<int> m_ViewportCount;
	StateMonitorArray<int, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> m_Viewports;

public:
	DX11RasterizerStateStageState();
	~DX11RasterizerStateStageState();

	// IN: DX11RasterizerStateState* - pointer to the previous state
	// OUT: void
	//
	// Sets the previous state for all monitors
	void SetPreviousState(DX11RasterizerStateStageState* previous);
	// IN: void
	// OUT: void
	//
	// Clears all the states to the inital state
	void ClearState();
	// IN: void
	// OUT: void
	//
	// Resets all the monitors update flags
	void ResetUpdateFlages();
};

