#pragma once

#include <d3d11.h>

#include "DX11Stage.h"

class DX11PixelShaderStage : public DX11Stage
{
public:
	DX11PixelShaderStage();
	~DX11PixelShaderStage();

	virtual void BindShader(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources) override;
	virtual void BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources) override;
};

