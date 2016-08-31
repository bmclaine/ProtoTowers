#pragma once

#include "DX11RasterizerStateStageState.h"

class RenderingResources;

class DX11RasterizerStateStage
{
private:
	DX11RasterizerStateStageState CurrentState;

public:
	DX11RasterizerStateStageState DesiredState;

public:
	DX11RasterizerStateStage();
	~DX11RasterizerStateStage();

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

	// IN: void
	// OUT: DX11RasterizerStateState& - reference to the current rasterizer state
	//
	// Returns a reference to the current rasterizer state
	const DX11RasterizerStateStageState& GetCurrentState() const;
};

