#pragma once

#include "ShaderInfo.h"

#include <d3d11.h>
#include <string>

class DX11Buffer
{
public:
	std::wstring m_sName;
	ID3D11Buffer* m_pBuffer;

public:
	DX11Buffer();
	virtual ~DX11Buffer();

	ID3D11Buffer* GetBuffer() const;
	virtual BufferType GetType() const = 0;
};

