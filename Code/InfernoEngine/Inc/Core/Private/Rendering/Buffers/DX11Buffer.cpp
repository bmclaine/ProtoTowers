#include "..\..\..\Public\Rendering\Buffers\DX11Buffer.h"

DX11Buffer::DX11Buffer()
{
}

DX11Buffer::~DX11Buffer()
{
}

ID3D11Buffer* DX11Buffer::GetBuffer() const { return m_pBuffer; }