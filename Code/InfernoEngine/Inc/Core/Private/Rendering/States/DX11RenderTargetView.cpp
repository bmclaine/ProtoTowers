#include "..\..\..\Public\Rendering\States\DX11RenderTargetView.h"

DX11RenderTargetView::DX11RenderTargetView(ID3D11RenderTargetView* rtv)
{
	m_pRTV = rtv;
}


DX11RenderTargetView::~DX11RenderTargetView()
{
	if (m_pRTV)
	{
		m_pRTV->Release();
		m_pRTV = nullptr;
	}
}