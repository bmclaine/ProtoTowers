#include "..\..\..\Public\Rendering\Shaders\DX11ComputeShader.h"

DX11ComputeShader::DX11ComputeShader(ID3D11ComputeShader* shader)
{
	m_pShader = shader;
}

DX11ComputeShader::~DX11ComputeShader()
{
	if (m_pShader)
	{
		m_pShader->Release();
		m_pShader = nullptr;
	}
}

ShaderType DX11ComputeShader::GetType() const { return COMPUTE_SHADER; }