#pragma once

#include <d3d11.h>

#include "Rendering\Common\StateMonitor.h"
#include "Rendering\Common\StateMonitorArray.h"

class DX11InputAssemblerStageState
{
public:
	StateMonitor<int> m_IndexBuffer;
	StateMonitor<DXGI_FORMAT> m_IndexBufferFormat;
	StateMonitorArray<int, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_VertexBuffers;
	StateMonitorArray<unsigned int, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_VertexBufferStrides;
	StateMonitorArray<unsigned int, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> m_VertexBufferOffsets;
	StateMonitor<int> m_InputLayout;
	StateMonitor<D3D11_PRIMITIVE_TOPOLOGY> m_PrimitiveTopology;

public:
	DX11InputAssemblerStageState();
	~DX11InputAssemblerStageState();

	// IN: DX11InputAssemblerState* - pointer to the previous state
	// OUT: void
	//
	// Sets the previous state for all monitors
	void SetPreviousState(DX11InputAssemblerStageState* pState);
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

	unsigned int GetAvaiableSlotCount();

protected:
	unsigned int m_uiAviableSlotCount;
};

