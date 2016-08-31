#include "Rendering\Shaders\DX11GeometryShader.h"

DX11GeometryShader::DX11GeometryShader(ID3D11GeometryShader* shader)
{
	m_pShader = shader;
}


DX11GeometryShader::~DX11GeometryShader()
{
	if (m_pShader)
	{
		m_pShader->Release();
		m_pShader = nullptr;
	}
}

ShaderType DX11GeometryShader::GetType() const { return GEOMETRY_SHADER; }