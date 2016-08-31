#include "Rendering\States\DX11ShaderResourceView.h"

DX11ShaderResourceView::DX11ShaderResourceView(ID3D11ShaderResourceView* image)
{
	m_pSRV = image;
}

DX11ShaderResourceView::~DX11ShaderResourceView()
{
	if (m_pSRV)
	{
		m_pSRV->Release();
		m_pSRV = nullptr;
	}
}

void DX11ShaderResourceView::SetType(SHADER_RESOURCE_TYPE type) { m_eType = type; }

SHADER_RESOURCE_TYPE DX11ShaderResourceView::GetType() const { return m_eType; }