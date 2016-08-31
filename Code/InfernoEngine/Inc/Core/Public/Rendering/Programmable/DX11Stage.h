#pragma once

#include "DX11ShaderStageState.h"

class RenderingResources;

class DX11Stage
{
protected:
	DX11ShaderStageState CurrentState;

public:
	DX11Stage();
	~DX11Stage();

	// IN: ID3D11DeviceContext* - pointer to the current context
	// OUT: void
	//
	// Applies the current state and any changes that need to be made
	void ApplyDesiredState(ID3D11DeviceContext* context, RenderingResources* resources);
	// IN: void
	// OUT: void
	//
	// Resets our desired state
	void ClearDesiredState();
	// IN: void
	// OUT: void
	//
	// Resets our current state
	void ClearCurrentState();

	virtual void BindShader(ID3D11DeviceContext* context, RenderingResources* resources) = 0;
	virtual void BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources) = 0;
	virtual void BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources) = 0;
	virtual void BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources) = 0;
	virtual void BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources) = 0;

	DX11ShaderStageState DesiredState;
};

