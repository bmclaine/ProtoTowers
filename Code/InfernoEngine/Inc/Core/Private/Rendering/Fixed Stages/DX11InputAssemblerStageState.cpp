#include "..\..\..\Public\Rendering\Fixed Stages\DX11InputAssemblerStageState.h"
#include "..\Public\Assets\Mesh.h"

DX11InputAssemblerStageState::DX11InputAssemblerStageState() : m_IndexBuffer(0), m_IndexBufferFormat(DXGI_FORMAT_R32_UINT), m_VertexBuffers(0),
m_VertexBufferStrides(sizeof(Vertex)), m_VertexBufferOffsets(0), m_InputLayout(0), m_PrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED), m_uiAviableSlotCount(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
{
	ClearState();
}

DX11InputAssemblerStageState::~DX11InputAssemblerStageState()
{
}

// IN: DX11InputAssemblerState* - pointer to the previous state
// OUT: void
//
// Sets the previous state for all monitors
void DX11InputAssemblerStageState::SetPreviousState(DX11InputAssemblerStageState* pState)
{
	m_IndexBuffer.SetPrevious(&pState->m_IndexBuffer);
	m_IndexBufferFormat.SetPrevious(&pState->m_IndexBufferFormat);
	m_VertexBuffers.SetPrevious(&pState->m_VertexBuffers);
	m_VertexBufferStrides.SetPrevious(&pState->m_VertexBufferStrides);
	m_VertexBufferOffsets.SetPrevious(&pState->m_VertexBufferOffsets);
	m_InputLayout.SetPrevious(&pState->m_InputLayout);
	m_PrimitiveTopology.SetPrevious(&pState->m_PrimitiveTopology);
}

// IN: void
// OUT: void
//
// Clears all the states to the inital state
void DX11InputAssemblerStageState::ClearState()
{
	m_IndexBuffer.InitializeState();
	m_IndexBufferFormat.InitializeState();
	m_VertexBuffers.InitializeState();
	m_VertexBufferStrides.InitializeState();
	m_VertexBufferOffsets.InitializeState();
	m_InputLayout.InitializeState();
	m_PrimitiveTopology.InitializeState();
}

// IN: void
// OUT: void
//
// Resets all the monitors update flags
void DX11InputAssemblerStageState::ResetUpdateFlages()
{
	m_IndexBuffer.ResetTracking();
	m_IndexBufferFormat.ResetTracking();
	m_VertexBuffers.ResetTracking();
	m_VertexBufferStrides.ResetTracking();
	m_VertexBufferOffsets.ResetTracking();
	m_InputLayout.ResetTracking();
	m_PrimitiveTopology.ResetTracking();
}

unsigned int DX11InputAssemblerStageState::GetAvaiableSlotCount() { return m_uiAviableSlotCount; }