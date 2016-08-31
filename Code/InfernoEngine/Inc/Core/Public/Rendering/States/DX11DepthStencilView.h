#pragma once

#include <d3d11.h>
#include <string>

class DX11DepthStencilView
{
private:
	ID3D11DepthStencilView* m_pDSV;

public:
	std::wstring m_sName;

public:
	DX11DepthStencilView(ID3D11DepthStencilView* dsv);
	~DX11DepthStencilView();

	friend class PipelineManager;
	friend class DX11OutputMergerStage;
	friend class GraphicsManager;
};

