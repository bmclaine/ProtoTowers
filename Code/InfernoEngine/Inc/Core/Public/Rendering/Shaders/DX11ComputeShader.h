#pragma once

#include "DX11Shader.h"

class DX11ComputeShader : public DX11Shader
{
private:
	ID3D11ComputeShader* m_pShader;

public:
	DX11ComputeShader(ID3D11ComputeShader* shader);
	virtual ~DX11ComputeShader();

	virtual ShaderType GetType() const override;

	friend class DX11ComputeShaderStage;
};

