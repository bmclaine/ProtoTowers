#pragma once

#include "ShaderInfo.h"
#include "Rendering\Buffers\DX11ConstantBuffer.h"

#include <d3d11.h>
#include <string>
#include <vector>

class DX11Shader
{
protected:
	std::vector<UINT> m_vBufferSlots;
	std::vector<DX11ConstantBuffer*> m_vConstantBuffers;

public:
	std::wstring m_sShaderName;
	ID3DBlob* m_pCompiledShader;

public:
	DX11Shader();
	virtual ~DX11Shader();

	void AddConstantBuffer(UINT slot, DX11ConstantBuffer* cBuffer);

	virtual ShaderType GetType() const = 0;

	friend class PipelineManager;
};

