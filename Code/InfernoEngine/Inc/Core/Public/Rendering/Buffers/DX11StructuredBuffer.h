#pragma once

#include "DX11Buffer.h"

class DX11StructuredBuffer : public DX11Buffer
{
public:
	DX11StructuredBuffer(ID3D11Buffer* buffer);
	virtual ~DX11StructuredBuffer();

	virtual BufferType GetType() const override;
};

