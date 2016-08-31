#include "Rendering\States\DX11RasterizerState.h"

DX11RasterizerState::DX11RasterizerState(ID3D11RasterizerState* state)
{
	m_pRasterizerState = state;
}

DX11RasterizerState::~DX11RasterizerState()
{
	if (m_pRasterizerState)
	{
		m_pRasterizerState->Release();
		m_pRasterizerState = nullptr;
	}
}
