#pragma once

#include <d3d11.h>

#include "Rendering\Common\StateMonitor.h"
#include "Rendering\Common\StateMonitorArray.h"

class DX11OutputMergerStageState
{
public:
	StateMonitor<int> m_BlendState;
	StateMonitor<int> m_DepthStencilState;
	StateMonitor<unsigned int> m_DepthStencilRef;
	StateMonitorArray<int, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> m_RenderTargetViews;
	StateMonitor<int> m_DepthStencilView;

public:
	DX11OutputMergerStageState();
	~DX11OutputMergerStageState();

	// IN: DX11OutputMergerState* - pointer to the previous state
	// OUT: void
	//
	// Sets the previous state for all monitors
	void SetPreviousState(DX11OutputMergerStageState* previous);
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

	// IN: void
	// OUT: int - number of total render targets
	//
	// Returns the current number of render targets active
	int GetRenderTargetCount() const;
};

