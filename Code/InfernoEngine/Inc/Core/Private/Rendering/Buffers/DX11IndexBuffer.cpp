#include "Rendering\Buffers\DX11IndexBuffer.h"

DX11IndexBuffer::DX11IndexBuffer(ID3D11Buffer* buffer)
{
	m_pBuffer = buffer;
}

DX11IndexBuffer::~DX11IndexBuffer()
{
	if (m_pBuffer)
	{
		m_pBuffer->Release();
		m_pBuffer = nullptr;
	}
}

BufferType DX11IndexBuffer::GetType() const { return INDEX_BUFFER; }