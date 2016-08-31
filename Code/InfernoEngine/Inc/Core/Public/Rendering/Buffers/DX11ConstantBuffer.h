#pragma once

#include "DX11Buffer.h"

class DX11ConstantBuffer : public DX11Buffer
{
public:
	DX11ConstantBuffer(ID3D11Buffer* buffer);
	virtual ~DX11ConstantBuffer();

	virtual BufferType GetType() const;

	friend class DX11VertexShaderStage;
	friend class DX11GeometryShaderStage;
	friend class DX11PixelShaderStage;
	friend class PipelineManager;
};