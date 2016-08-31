#pragma once

#include "DX11Buffer.h"

class DX11IndexBuffer :	public DX11Buffer
{
public:
	DX11IndexBuffer(ID3D11Buffer* buffer);
	virtual ~DX11IndexBuffer();

	virtual BufferType GetType() const;

	friend class DX11InputAssemblerStage;
};

