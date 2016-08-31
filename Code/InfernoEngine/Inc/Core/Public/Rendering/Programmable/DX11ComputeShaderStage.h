#pragma once

#include "DX11Stage.h"

#include <d3d11.h>

class DX11ComputeShaderStage : public DX11Stage
{
public:
	DX11ComputeShaderStage();
	~DX11ComputeShaderStage();

	virtual void BindShader(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources) override;
};

