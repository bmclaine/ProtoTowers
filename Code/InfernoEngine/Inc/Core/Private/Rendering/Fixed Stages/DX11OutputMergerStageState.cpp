#include "..\..\..\Public\Rendering\Fixed Stages\DX11OutputMergerStageState.h"

DX11OutputMergerStageState::DX11OutputMergerStageState() : m_BlendState(-1), m_DepthStencilState(-1), m_DepthStencilRef(0), m_RenderTargetViews(-1), m_DepthStencilView(-1)
{
}

DX11OutputMergerStageState::~DX11OutputMergerStageState()
{
}

// IN: DX11OutputMergerState* - pointer to the previous state
// OUT: void
//
// Sets the previous state for all monitors
void DX11OutputMergerStageState::SetPreviousState(DX11OutputMergerStageState* previous)
{
	m_BlendState.SetPrevious(&previous->m_BlendState);
	m_DepthStencilState.SetPrevious(&previous->m_DepthStencilState);
	m_DepthStencilRef.SetPrevious(&previous->m_DepthStencilRef);
	m_RenderTargetViews.SetPrevious(&previous->m_RenderTargetViews);
	m_DepthStencilView.SetPrevious(&previous->m_DepthStencilView);
}
// IN: void
// OUT: void
//
// Clears all the states to the inital state
void DX11OutputMergerStageState::ClearState()
{
	m_BlendState.InitializeState();
	m_DepthStencilState.InitializeState();
	m_DepthStencilRef.InitializeState();
	m_RenderTargetViews.InitializeState();
	m_DepthStencilView.InitializeState();
}
// IN: void
// OUT: void
//
// Resets all the monitors update flags
void DX11OutputMergerStageState::ResetUpdateFlages()
{
	m_BlendState.ResetTracking();
	m_DepthStencilState.ResetTracking();
	m_DepthStencilRef.ResetTracking();
	m_RenderTargetViews.ResetTracking();
	m_DepthStencilView.ResetTracking();
}

// IN: void
// OUT: int - number of total render targets
//
// Returns the current number of render targets active
int DX11OutputMergerStageState::GetRenderTargetCount() const
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		if (m_RenderTargetViews.GetState(i) != -1)
			count++;
	}

	return count;
}