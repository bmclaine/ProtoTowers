#include "..\..\..\Public\Rendering\Buffers\DX11VertexBuffer.h"

DX11VertexBuffer::DX11VertexBuffer(ID3D11Buffer* buffer)
{
	m_pBuffer = buffer;
}

DX11VertexBuffer::~DX11VertexBuffer()
{
	if (m_pBuffer)
	{
		m_pBuffer->Release();
		m_pBuffer = nullptr;
	}
}

BufferType DX11VertexBuffer::GetType() const { return VERTEX_BUFFER; }