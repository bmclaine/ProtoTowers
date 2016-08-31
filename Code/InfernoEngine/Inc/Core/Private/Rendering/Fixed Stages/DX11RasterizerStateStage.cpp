#include "Rendering\Fixed Stages\DX11RasterizerStateStage.h"
#include "Rendering\RenderingResources.h"
#include "GraphicsManager.h"

DX11RasterizerStateStage::DX11RasterizerStateStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11RasterizerStateStage::~DX11RasterizerStateStage()
{
}

// IN: void
// OUT: void
//
// Clears the desired state
void DX11RasterizerStateStage::ClearDesiredState()
{
	DesiredState.ClearState();
}
// IN: void
// OUT: void
//
// Clears the current state
void DX11RasterizerStateStage::ClearCurrentState()
{
	CurrentState.ClearState();
}
// IN: GraphicsManager* - manger to use to apply changes to the pipeline
// OUT: void
//
// Applies any changes to the current pipeline
void DX11RasterizerStateStage::ApplyDesiredState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	if (DesiredState.m_RasterizerState.UpdateNeeded())
	{
		DX11RasterizerState* rState = resources->GetRasterizerStateByID(DesiredState.m_RasterizerState.GetState());

		if (rState)
			context->RSSetState(rState->m_pRasterizerState);
		else
			context->RSSetState(nullptr);
	}

	if (DesiredState.m_Viewports.UpdateNeeded() ||
		DesiredState.m_ViewportCount.UpdateNeeded())
	{
		D3D11_VIEWPORT viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

		for (int i = 0; i < DesiredState.m_ViewportCount.GetState(); i++)
			viewports[i] = *resources->GetViewportByID(DesiredState.m_Viewports.GetState(i));

		context->RSSetViewports(DesiredState.m_ViewportCount.GetState(), viewports);
	}
}

// IN: void
// OUT: DX11RasterizerStateState& - reference to the current rasterizer state
//
// Returns a reference to the current rasterizer state
const DX11RasterizerStateStageState& DX11RasterizerStateStage::GetCurrentState() const { return CurrentState; }