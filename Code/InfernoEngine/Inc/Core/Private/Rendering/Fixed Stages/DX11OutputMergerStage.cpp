#include "..\..\..\Public\Rendering\Fixed Stages\DX11OutputMergerStage.h"
#include "Rendering\RenderingResources.h"
#include "GraphicsManager.h"

DX11OutputMergerStage::DX11OutputMergerStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11OutputMergerStage::~DX11OutputMergerStage()
{
}

// IN: void
// OUT: void
//
// Clears the desired state
void DX11OutputMergerStage::ClearDesiredState()
{
	DesiredState.ClearState();
}
// IN: void
// OUT: void
//
// Clears the current state
void DX11OutputMergerStage::ClearCurrentState()
{
	CurrentState.ClearState();
}
// IN: GraphicsManager* - manger to use to apply changes to the pipeline
// OUT: void
//
// Applies any changes to the current pipeline
void DX11OutputMergerStage::ApplyDesiredState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	ApplyDesiredRenderTargetViews(context, resources);
	ApplyDesiredBlendAndDepthStencilStates(context, resources);

	DesiredState.ResetUpdateFlages();
	CurrentState = DesiredState;
}
// IN: GraphicsManager* - manger to use to apply changes to the pipeline
// OUT: void
//
// Applies the changed render targets to the current pipeline
void DX11OutputMergerStage::ApplyDesiredRenderTargetViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	if (DesiredState.m_RenderTargetViews.UpdateNeeded() ||
		DesiredState.m_DepthStencilView.UpdateNeeded())
	{
		ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

		for (unsigned int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			DX11RenderTargetView* rtv = resources->GetRenderTargetViewByID(DesiredState.m_RenderTargetViews.GetState(i));

			if (rtv)
				rtvs[i] = rtv->m_pRTV;
		}

		DX11DepthStencilView* dsv = resources->GetDepthStencilViewByID(DesiredState.m_DepthStencilView.GetState());

		if (dsv)
			context->OMSetRenderTargets(DesiredState.GetRenderTargetCount(), rtvs, dsv->m_pDSV);
		else
			context->OMSetRenderTargets(DesiredState.GetRenderTargetCount(), rtvs, nullptr);
	}
}
// IN: GraphicsManager* - manger to use to apply changes to the pipeline
// OUT: void
//
// Applies the changed blend and depth stencil states to the current pipeline
void DX11OutputMergerStage::ApplyDesiredBlendAndDepthStencilStates(ID3D11DeviceContext* context, RenderingResources* resources)
{
	if (DesiredState.m_BlendState.UpdateNeeded())
	{
		DX11BlendState* state = resources->GetBlendStateByID(DesiredState.m_BlendState.GetState());
		ID3D11BlendState* blendState = nullptr;

		if (state)
			blendState = state->m_pBlendState;

		float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		context->OMSetBlendState(blendState, blendFactors, 0xFFFFFFFF);
	}

	if (DesiredState.m_DepthStencilState.UpdateNeeded() ||
		DesiredState.m_DepthStencilRef.UpdateNeeded())
	{
		DX11DepthStencilState* state = resources->GetDepthStencilStateByID(DesiredState.m_DepthStencilState.GetState());
		ID3D11DepthStencilState* DSS = nullptr;

		if (state)
			DSS = state->m_pDSS;

		context->OMSetDepthStencilState(DSS, DesiredState.m_DepthStencilRef.GetState());
	}
}

// IN: void
// OUT: DX11OutputMergerState& - reference to the current output merger state
//
// Returns a reference to the current output merger state
const DX11OutputMergerStageState& DX11OutputMergerStage::GetCurrentState() const { return CurrentState; }