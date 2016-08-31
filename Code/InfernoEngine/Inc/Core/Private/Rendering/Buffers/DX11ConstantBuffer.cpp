#include "Rendering\Buffers\DX11ConstantBuffer.h"

DX11ConstantBuffer::DX11ConstantBuffer(ID3D11Buffer* buffer)
{
	m_pBuffer = buffer;
}

DX11ConstantBuffer::~DX11ConstantBuffer()
{
	if (m_pBuffer)
	{
		m_pBuffer->Release();
		m_pBuffer = nullptr;
	}
}

BufferType DX11ConstantBuffer::GetType() const { return CONSTANT_BUFFER; }