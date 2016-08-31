#include "..\..\..\Public\Rendering\States\DX11SampleState.h"

DX11SampleState::DX11SampleState(ID3D11SamplerState* state)
{
	m_pSamplerState = state;
}


DX11SampleState::~DX11SampleState()
{
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = nullptr;
	}
}