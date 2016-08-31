#include "..\..\..\Public\Rendering\Programmable\DX11Stage.h"
#include "Rendering\RenderingResources.h"

DX11Stage::DX11Stage()
{
}

DX11Stage::~DX11Stage()
{
}

// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Applies the current state and any changes that need to be made
void DX11Stage::ApplyDesiredState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	if (DesiredState.m_Shader.UpdateNeeded())
		BindShader(context, resources);

	if (DesiredState.m_ConstantBuffers.UpdateNeeded())
		BindConstantBuffer(context, resources);
	
	if (DesiredState.m_SamplerStates.UpdateNeeded())
		BindSamplerState(context, resources);

	if (DesiredState.m_ShaderResourceViews.UpdateNeeded())
		BindShaderResourceViews(context, resources);

	if (DesiredState.m_UnorderedAccessViews.UpdateNeeded() ||
		DesiredState.m_UAVInitialCounts.UpdateNeeded())
		BindUnorderedAccessViews(context, resources);

	DesiredState.ResetUpdateFlags();

	CurrentState = DesiredState;
}

// IN: void
// OUT: void
//
// Resets our desired state
void DX11Stage::ClearDesiredState()
{
	DesiredState.ClearState();
	DesiredState.ResetUpdateFlags();
}

// IN: void
// OUT: void
//
// Resets our current state
void DX11Stage::ClearCurrentState()
{
	CurrentState.ClearState();
	CurrentState.ResetUpdateFlags();
}