#pragma once

#include <d3d11.h>
#include <string>

class DX11DepthStencilState
{
private:
	ID3D11DepthStencilState* m_pDSS;

public:
	std::wstring m_sName;

public:
	DX11DepthStencilState(ID3D11DepthStencilState* state);
	~DX11DepthStencilState();

	friend class PipelineManager;
	friend class DX11OutputMergerStage;
};

