#pragma once

#include "DX11Stage.h"

class DX11VertexShaderStage : public DX11Stage
{
public:
	DX11VertexShaderStage();
	~DX11VertexShaderStage();

	// IN: ID3D11DeviceContext* - pointer to the current context
	// OUT: void
	//
	// Binds the current shader set in Desired State if it needs to change
	virtual void BindShader(ID3D11DeviceContext* context, RenderingResources* resources) override;
	// IN: ID3D11DeviceContext* - pointer to the current context
	// OUT: void
	//
	// Binds the context buffers in the Desired State that need to change
	virtual void BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources) override;
	// IN: ID3D11DeviceContext* - pointer to the current context
	// OUT: void
	//
	// Binds the sampler states in the Desired State that need to change
	virtual void BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources) override;
	// IN: ID3D11DeviceContext* - pointer to the current context
	// OUT: void
	//
	// Binds the shader resource views in the Desired State that need to change
	virtual void BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources) override;
};

