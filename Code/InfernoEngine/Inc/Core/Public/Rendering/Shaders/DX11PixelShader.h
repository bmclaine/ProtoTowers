#pragma once

#include "DX11Shader.h"

class DX11PixelShader : public DX11Shader
{
private:
	ID3D11PixelShader* m_pShader;

public:
	DX11PixelShader(ID3D11PixelShader* shader);
	virtual ~DX11PixelShader();

	virtual ShaderType GetType() const;

	friend class DX11PixelShaderStage;
};

