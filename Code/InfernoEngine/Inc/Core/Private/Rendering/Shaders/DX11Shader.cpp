#include "Rendering\Shaders\DX11Shader.h"

DX11Shader::DX11Shader()
{
}


DX11Shader::~DX11Shader()
{
	if (m_pCompiledShader)
	{
		m_pCompiledShader->Release();
		m_pCompiledShader = nullptr;
	}
}

void DX11Shader::AddConstantBuffer(UINT slot, DX11ConstantBuffer* cBuffer)
{
	// Make sure we don't already have this buffer
	for (size_t buffer = 0; buffer < m_vConstantBuffers.size(); buffer++)
	{
		// If either the slot or the buffer are already assigned, don't add the buffer
		if (m_vBufferSlots[buffer] == slot || m_vConstantBuffers[buffer] == cBuffer)
			return;
	}

	m_vConstantBuffers.push_back(cBuffer);
	m_vBufferSlots.push_back(slot);
}