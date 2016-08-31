#pragma once

#include <d3d11.h>
#include <string>

class DX11RasterizerState
{
private:
	ID3D11RasterizerState* m_pRasterizerState;

public:
	std::wstring m_sName;

public:
	DX11RasterizerState(ID3D11RasterizerState* state);
	~DX11RasterizerState();

	friend class PipelineManager;
	friend class DX11RasterizerStateStage;
};

