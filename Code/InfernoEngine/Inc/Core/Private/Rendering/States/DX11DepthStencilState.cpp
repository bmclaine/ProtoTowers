#include "..\..\..\Public\Rendering\States\DX11DepthStencilState.h"

DX11DepthStencilState::DX11DepthStencilState(ID3D11DepthStencilState* state)
{
	m_pDSS = state;
}

DX11DepthStencilState::~DX11DepthStencilState()
{
	if (m_pDSS)
	{
		m_pDSS->Release();
		m_pDSS = nullptr;
	}
}