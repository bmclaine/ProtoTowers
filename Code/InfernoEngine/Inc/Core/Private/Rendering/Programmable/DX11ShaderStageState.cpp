#include "..\..\..\Public\Rendering\Programmable\DX11ShaderStageState.h"


DX11ShaderStageState::DX11ShaderStageState() : m_Shader(-1), m_ConstantBuffers(0), m_SamplerStates(0), m_ShaderResourceViews(0), m_UnorderedAccessViews(0), m_UAVInitialCounts(-1)
{
}

DX11ShaderStageState::~DX11ShaderStageState()
{
}

// IN: DX11ShaderStageState - state to consider the previous state
// OUT: void
//
// Sets the incoming state as our previous state
void DX11ShaderStageState::SetPreviousState(DX11ShaderStageState* previous)
{
	m_Shader.SetPrevious(&previous->m_Shader);
	m_ConstantBuffers.SetPrevious(&previous->m_ConstantBuffers);
	m_SamplerStates.SetPrevious(&previous->m_SamplerStates);
	m_ShaderResourceViews.SetPrevious(&previous->m_ShaderResourceViews);
	m_UnorderedAccessViews.SetPrevious(&previous->m_UnorderedAccessViews);
	m_UAVInitialCounts.SetPrevious(&previous->m_UAVInitialCounts);
}
// IN: void
// OUT: void
//
// Resets the flags for updating
void DX11ShaderStageState::ResetUpdateFlags()
{
	m_Shader.ResetTracking();
	m_ConstantBuffers.ResetTracking();
	m_SamplerStates.ResetTracking();
	m_ShaderResourceViews.ResetTracking();
	m_UnorderedAccessViews.ResetTracking();
	m_UAVInitialCounts.ResetTracking();
}
// IN: void
// OUT: void
//
// Clears the states to their initial state
void DX11ShaderStageState::ClearState()
{
	m_Shader.InitializeState();
	m_ConstantBuffers.InitializeState();
	m_SamplerStates.InitializeState();
	m_ShaderResourceViews.InitializeState();
	m_UnorderedAccessViews.InitializeState();
	m_UAVInitialCounts.InitializeState();
}