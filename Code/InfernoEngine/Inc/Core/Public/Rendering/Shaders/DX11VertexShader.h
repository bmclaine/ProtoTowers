#pragma once

#include "DX11Shader.h"

class DX11VertexShader : public DX11Shader
{
private:
	ID3D11VertexShader* m_pShader;

public:
	DX11VertexShader(ID3D11VertexShader* shader);
	virtual ~DX11VertexShader();

	virtual ShaderType GetType() const;

	friend class DX11VertexShaderStage;
};

