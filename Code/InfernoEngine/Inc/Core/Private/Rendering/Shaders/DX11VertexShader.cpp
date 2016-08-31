#include "..\..\..\Public\Rendering\Shaders\DX11VertexShader.h"


DX11VertexShader::DX11VertexShader(ID3D11VertexShader* shader)
{
	m_pShader = shader;
}


DX11VertexShader::~DX11VertexShader()
{
	if (m_pShader)
	{
		m_pShader->Release();
		m_pShader = nullptr;
	}
}

ShaderType DX11VertexShader::GetType() const { return VERTEX_SHADER; }