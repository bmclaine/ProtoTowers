#pragma once

#include <d3d11.h>
#include <string>

class DX11SampleState
{
private:
	ID3D11SamplerState* m_pSamplerState;

public:
	std::wstring m_sName;

public:
	DX11SampleState(ID3D11SamplerState* state);
	~DX11SampleState();

	friend class PipelineManager;
	friend class RenderingResources;
};

