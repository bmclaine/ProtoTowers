#include "..\..\..\Public\Rendering\States\DX11InputLayout.h"


DX11InputLayout::DX11InputLayout(ID3D11InputLayout* layout)
{
	m_pInputLayout = layout;
}

DX11InputLayout::~DX11InputLayout()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}
}