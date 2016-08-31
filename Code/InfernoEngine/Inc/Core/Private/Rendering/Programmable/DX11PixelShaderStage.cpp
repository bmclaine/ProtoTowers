#include "Rendering\Programmable\DX11PixelShaderStage.h"
#include "Rendering\Shaders\DX11PixelShader.h"
#include "Rendering\RenderingResources.h"

DX11PixelShaderStage::DX11PixelShaderStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11PixelShaderStage::~DX11PixelShaderStage()
{
}

// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the current shader set in Desired State if it needs to change
void DX11PixelShaderStage::BindShader(ID3D11DeviceContext* context, RenderingResources* resources)
{
	DX11PixelShader* shader = (DX11PixelShader*)resources->GetShaderByID(PIXEL_SHADER, DesiredState.m_Shader.GetState());

	if (shader)
		context->PSSetShader(shader->m_pShader, nullptr, 0);
	else
		context->PSSetShader(nullptr, nullptr, 0);
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the context buffers in the Desired State that need to change
void DX11PixelShaderStage::BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->PSSetConstantBuffers(
		DesiredState.m_ConstantBuffers.GetStartSlot(),
		DesiredState.m_ConstantBuffers.GetRange(),
		DesiredState.m_ConstantBuffers.GetFirstSlot());
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the sampler states in the Desired State that need to change
void DX11PixelShaderStage::BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->PSSetSamplers(
		DesiredState.m_SamplerStates.GetStartSlot(),
		DesiredState.m_SamplerStates.GetRange(),
		DesiredState.m_SamplerStates.GetFirstSlot());
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the shader resource views in the Desired State that need to change
void DX11PixelShaderStage::BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->PSSetShaderResources(
		DesiredState.m_ShaderResourceViews.GetStartSlot(),
		DesiredState.m_ShaderResourceViews.GetRange(),
		DesiredState.m_ShaderResourceViews.GetFirstSlot());
}
void DX11PixelShaderStage::BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	// Do nothing even though the pixel shader supports Unordered Access Views
}