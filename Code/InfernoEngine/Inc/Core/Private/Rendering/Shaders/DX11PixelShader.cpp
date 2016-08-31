#include "Rendering\Shaders\DX11PixelShader.h"

DX11PixelShader::DX11PixelShader(ID3D11PixelShader* shader)
{
	m_pShader = shader;
}


DX11PixelShader::~DX11PixelShader()
{
	if (m_pShader)
	{
		m_pShader->Release();
		m_pShader = nullptr;
	}
}

ShaderType DX11PixelShader::GetType() const { return PIXEL_SHADER; }