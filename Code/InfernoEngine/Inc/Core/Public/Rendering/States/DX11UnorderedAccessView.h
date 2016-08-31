#pragma once

#include <d3d11.h>
#include <string>

class DX11UnorderedAccessView
{
private:
	ID3D11UnorderedAccessView* m_pUAV;

public:
	std::wstring m_sName;

public:
	DX11UnorderedAccessView(ID3D11UnorderedAccessView* uav);
	~DX11UnorderedAccessView();

	friend class PipelineManager;
	friend class RenderingResources;
	friend class ParticleManager;
};

