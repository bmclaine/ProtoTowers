#pragma once

#include <d3d11.h>
#include <string>

class DX11RenderTargetView
{
private:
	ID3D11RenderTargetView* m_pRTV;

public:
	std::wstring m_sName;

public:
	DX11RenderTargetView(ID3D11RenderTargetView* rtv);
	~DX11RenderTargetView();

	friend class PipelineManager;
	friend class DX11OutputMergerStage;
	friend class GraphicsManager;
};

