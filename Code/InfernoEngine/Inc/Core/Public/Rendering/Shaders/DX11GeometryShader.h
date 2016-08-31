#pragma once

#include "DX11Shader.h"

class DX11GeometryShader : public DX11Shader
{
private:
	ID3D11GeometryShader* m_pShader;

public:
	DX11GeometryShader(ID3D11GeometryShader* shader);
	virtual ~DX11GeometryShader();

	virtual ShaderType GetType() const;

	friend class DX11GeometryShaderStage;
	friend class RenderingResources;
};

