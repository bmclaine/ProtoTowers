#include "..\..\..\Public\Rendering\States\DX11BlendState.h"

DX11BlendState::DX11BlendState(ID3D11BlendState* state)
{
	m_pBlendState = state;
}

DX11BlendState::~DX11BlendState()
{
	if (m_pBlendState)
	{
		m_pBlendState->Release();
		m_pBlendState = nullptr;
	}
}
