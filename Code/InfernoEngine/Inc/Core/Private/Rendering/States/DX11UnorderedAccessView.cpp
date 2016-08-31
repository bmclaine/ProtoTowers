#include "Rendering\States\DX11UnorderedAccessView.h"

DX11UnorderedAccessView::DX11UnorderedAccessView(ID3D11UnorderedAccessView* uav)
{
	m_pUAV = uav;
}

DX11UnorderedAccessView::~DX11UnorderedAccessView()
{
	if (m_pUAV)
	{
		m_pUAV->Release();
		m_pUAV = nullptr;
	}
}