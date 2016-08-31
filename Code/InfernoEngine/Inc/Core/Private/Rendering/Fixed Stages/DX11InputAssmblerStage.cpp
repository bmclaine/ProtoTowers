#include "Rendering\Fixed Stages\DX11InputAssemblerStage.h"
#include "Rendering\RenderingResources.h"
#include "GraphicsManager.h"

#include "Rendering\Buffers\DX11VertexBuffer.h"
#include "Rendering\Buffers\DX11IndexBuffer.h"

DX11InputAssemblerStage::DX11InputAssemblerStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11InputAssemblerStage::~DX11InputAssemblerStage()
{
}

// IN: void
// OUT: void
//
// Clears the desired state
void DX11InputAssemblerStage::ClearDesiredState()
{
	DesiredState.ClearState();
}

// IN: void
// OUT: void
//
// Clears the current state
void DX11InputAssemblerStage::ClearCurrentState()
{
	CurrentState.ClearState();
}
// IN: GraphicsManager* - manger to use to apply changes to the pipeline
// OUT: void
//
// Applies any changes to the current pipeline
void DX11InputAssemblerStage::ApplyDesiredState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	if (DesiredState.m_VertexBuffers.UpdateNeeded() ||
		DesiredState.m_VertexBufferStrides.UpdateNeeded() ||
		DesiredState.m_VertexBufferOffsets.UpdateNeeded())
	{
		ID3D11Buffer* buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

		for (unsigned int i = 0; i < DesiredState.GetAvaiableSlotCount(); i++)
		{
			DX11VertexBuffer* buffer = (DX11VertexBuffer*)resources->GetBufferByID(VERTEX_BUFFER, DesiredState.m_VertexBuffers.GetState(i));

			if (buffer)
				buffers[i] = buffer->m_pBuffer;
			else
				buffers[i] = 0;
		}

		UINT startSlot = min(DesiredState.m_VertexBuffers.GetStartSlot(), min(DesiredState.m_VertexBufferOffsets.GetStartSlot(), DesiredState.m_VertexBufferStrides.GetStartSlot()));
		UINT endSlot = max(DesiredState.m_VertexBuffers.GetEndSlot(), max(DesiredState.m_VertexBufferOffsets.GetEndSlot(), DesiredState.m_VertexBufferStrides.GetEndSlot()));

		context->IASetVertexBuffers(startSlot, (endSlot - startSlot) + 1, &buffers[startSlot],
			DesiredState.m_VertexBufferStrides.GetSlotLocation(startSlot), DesiredState.m_VertexBufferOffsets.GetSlotLocation(startSlot));
	}

	if (DesiredState.m_IndexBuffer.UpdateNeeded() ||
		DesiredState.m_IndexBufferFormat.UpdateNeeded())
	{
		DX11IndexBuffer* buffer = (DX11IndexBuffer*)resources->GetBufferByID(INDEX_BUFFER, DesiredState.m_IndexBuffer.GetState());

		if (buffer)
			context->IASetIndexBuffer(buffer->m_pBuffer, DesiredState.m_IndexBufferFormat.GetState(), 0);
		else
			context->IASetIndexBuffer(nullptr, DesiredState.m_IndexBufferFormat.GetState(), 0);
	}

	if (DesiredState.m_InputLayout.UpdateNeeded())
	{
		DX11InputLayout* layout = resources->GetInputLayouByID(DesiredState.m_InputLayout.GetState());

		if (layout)
			context->IASetInputLayout(layout->m_pInputLayout);
		else
			context->IASetInputLayout(nullptr);
	}

	if (DesiredState.m_PrimitiveTopology.UpdateNeeded())
		context->IASetPrimitiveTopology(DesiredState.m_PrimitiveTopology.GetState());

	DesiredState.ResetUpdateFlages();
	CurrentState = DesiredState;
}

// IN: void
// OUT: DX11InputAssemblerState& - reference to the current input assembler state
//
// Returns a reference to the current input assembler state
const DX11InputAssemblerStageState& DX11InputAssemblerStage::GetCurrentState() const { return CurrentState; }