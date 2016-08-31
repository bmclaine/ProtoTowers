#include "..\..\..\Public\Rendering\States\DX11DepthStencilView.h"


DX11DepthStencilView::DX11DepthStencilView(ID3D11DepthStencilView* dsv)
{
	m_pDSV = dsv;
}

DX11DepthStencilView::~DX11DepthStencilView()
{
	if (m_pDSV)
	{
		m_pDSV->Release();
		m_pDSV = nullptr;
	}
}