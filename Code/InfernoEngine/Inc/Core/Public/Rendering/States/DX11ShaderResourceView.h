#pragma once

#include <d3d11.h>
#include <string>

#include "ShaderInfo.h"

class DX11ShaderResourceView
{
private:
	ID3D11ShaderResourceView* m_pSRV;
	SHADER_RESOURCE_TYPE m_eType;

public:
	std::wstring m_sName;

private:
	void SetType(SHADER_RESOURCE_TYPE type);

public:
	DX11ShaderResourceView(ID3D11ShaderResourceView* image);
	~DX11ShaderResourceView();

	SHADER_RESOURCE_TYPE GetType() const;

	friend class PipelineManager;
	friend class RenderingResources;
	friend class GraphicsManager;
};

