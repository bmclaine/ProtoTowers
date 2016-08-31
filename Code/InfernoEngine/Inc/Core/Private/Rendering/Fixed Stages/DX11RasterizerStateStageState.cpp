#include "Rendering\Fixed Stages\DX11RasterizerStateStageState.h"

DX11RasterizerStateStageState::DX11RasterizerStateStageState() : m_RasterizerState(-1), m_ViewportCount(0), m_Viewports(-1)
{
	ClearState();
}

DX11RasterizerStateStageState::~DX11RasterizerStateStageState()
{
}

// IN: DX11RasterizerStateState* - pointer to the previous state
// OUT: void
//
// Sets the previous state for all monitors
void DX11RasterizerStateStageState::SetPreviousState(DX11RasterizerStateStageState* previous)
{
	m_RasterizerState.SetPrevious(&previous->m_RasterizerState);
	m_ViewportCount.SetPrevious(&previous->m_ViewportCount);
	m_Viewports.SetPrevious(&previous->m_Viewports);
}
// IN: void
// OUT: void
//
// Clears all the states to the inital state
void DX11RasterizerStateStageState::ClearState()
{
	m_RasterizerState.InitializeState();
	m_ViewportCount.InitializeState();
	m_Viewports.InitializeState();
}
// IN: void
// OUT: void
//
// Resets all the monitors update flags
void DX11RasterizerStateStageState::ResetUpdateFlages()
{
	m_RasterizerState.ResetTracking();
	m_ViewportCount.ResetTracking();
	m_Viewports.ResetTracking();
}