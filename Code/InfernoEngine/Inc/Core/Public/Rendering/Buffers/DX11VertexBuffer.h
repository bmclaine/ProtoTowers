#pragma once

#include "DX11Buffer.h"

class DX11VertexBuffer : public DX11Buffer
{
public:
	DX11VertexBuffer(ID3D11Buffer* buffer);
	virtual ~DX11VertexBuffer();

	virtual BufferType GetType() const;

	friend class DX11InputAssemblerStage;
};

