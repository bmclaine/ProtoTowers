#pragma once

#include <d3d11.h>
#include <string>

class DX11InputLayout
{
private:
	ID3D11InputLayout* m_pInputLayout;

public:
	std::wstring m_sName;

public:
	DX11InputLayout(ID3D11InputLayout* layout);
	~DX11InputLayout();

	friend class PipelineManager;
	friend class DX11InputAssemblerStage;
};

