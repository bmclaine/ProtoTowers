#pragma once

#include <d3d11.h>

#include "Rendering\Common\StateMonitor.h"
#include "Rendering\Common\StateMonitorArray.h"

class DX11ShaderStageState
{
public:
	StateMonitor<int> m_Shader;
	StateMonitorArray<ID3D11Buffer*, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT> m_ConstantBuffers;
	StateMonitorArray<ID3D11SamplerState*, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT> m_SamplerStates;
	StateMonitorArray<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> m_ShaderResourceViews;
	StateMonitorArray<ID3D11UnorderedAccessView*, D3D11_PS_CS_UAV_REGISTER_COUNT> m_UnorderedAccessViews;
	StateMonitorArray<unsigned int, D3D11_PS_CS_UAV_REGISTER_COUNT> m_UAVInitialCounts;

public:
	DX11ShaderStageState();
	~DX11ShaderStageState();

	// IN: DX11ShaderStageState - state to consider the previous state
	// OUT: void
	//
	// Sets the incoming state as our previous state
	void SetPreviousState(DX11ShaderStageState* previous);
	// IN: void
	// OUT: void
	//
	// Resets the flags for updating
	void ResetUpdateFlags();
	// IN: void
	// OUT: void
	//
	// Clears the states to their initial state
	void ClearState();
};