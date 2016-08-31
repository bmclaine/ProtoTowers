#pragma once

#include "DX11InputAssemblerStageState.h"

class RenderingResources;

class DX11InputAssemblerStage
{
private:
	DX11InputAssemblerStageState CurrentState;

public:
	DX11InputAssemblerStageState DesiredState;

public:
	DX11InputAssemblerStage();
	~DX11InputAssemblerStage();

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
	// OUT: DX11InputAssemblerState& - reference to the current input assembler state
	//
	// Returns a reference to the current input assembler state
	const DX11InputAssemblerStageState& GetCurrentState() const;
};

