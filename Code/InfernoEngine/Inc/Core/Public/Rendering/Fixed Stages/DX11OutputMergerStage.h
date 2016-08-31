#pragma once

#include "Rendering\Fixed Stages\DX11OutputMergerStageState.h"

class RenderingResources;

class DX11OutputMergerStage
{
private:
	DX11OutputMergerStageState CurrentState;

public:
	DX11OutputMergerStageState DesiredState;

public:
	DX11OutputMergerStage();
	~DX11OutputMergerStage();

	// IN: void
	// OUT: void
	//
	// Clears the desired state
	void ClearDesiredState();
	// IN: void
	// OUT: void
	//
	// Clears the current state
	void ClearCurrentState();
	// IN: GraphicsManager* - manger to use to apply changes to the pipeline
	// OUT: void
	//
	// Applies any changes to the current pipeline
	void ApplyDesiredState(ID3D11DeviceContext* context, RenderingResources* resources);
	// IN: GraphicsManager* - manger to use to apply changes to the pipeline
	// OUT: void
	//
	// Applies the changed render targets to the current pipeline
	void ApplyDesiredRenderTargetViews(ID3D11DeviceContext* context, RenderingResources* resources);
	// IN: GraphicsManager* - manger to use to apply changes to the pipeline
	// OUT: void
	//
	// Applies the changed blend and depth stencil states to the current pipeline
	void ApplyDesiredBlendAndDepthStencilStates(ID3D11DeviceContext* context, RenderingResources* resources);

	// IN: void
	// OUT: DX11OutputMergerState& - reference to the current output merger state
	//
	// Returns a reference to the current output merger state
	const DX11OutputMergerStageState& GetCurrentState() const;
};

