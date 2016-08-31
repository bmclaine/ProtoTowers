#pragma once

#include <d3d11.h>
#include <string>

class DX11BlendState
{
private:
	ID3D11BlendState* m_pBlendState;

public:
	std::wstring m_sName;
	bool m_bAlphaBlending;

public:
	DX11BlendState(ID3D11BlendState* state);
	~DX11BlendState();

	friend class PipelineManager;
	friend class DX11OutputMergerStage;
};

