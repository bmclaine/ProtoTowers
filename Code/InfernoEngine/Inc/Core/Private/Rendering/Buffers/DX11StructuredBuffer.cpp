#include "Rendering\Buffers\DX11StructuredBuffer.h"

DX11StructuredBuffer::DX11StructuredBuffer(ID3D11Buffer* buffer)
{
	m_pBuffer = buffer;
}

DX11StructuredBuffer::~DX11StructuredBuffer()
{
	if (m_pBuffer)
	{
		m_pBuffer->Release();
		m_pBuffer = nullptr;
	}
}

BufferType DX11StructuredBuffer::GetType() const { return STRUCTURED_BUFFER; }