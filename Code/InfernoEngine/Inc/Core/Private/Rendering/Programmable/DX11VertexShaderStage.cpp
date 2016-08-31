#include "Rendering\Programmable\DX11VertexShaderStage.h"
#include "Rendering\Shaders\DX11VertexShader.h"
#include "Rendering\RenderingResources.h"

DX11VertexShaderStage::DX11VertexShaderStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11VertexShaderStage::~DX11VertexShaderStage()
{
}

// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the current shader set in Desired State if it needs to change
void DX11VertexShaderStage::BindShader(ID3D11DeviceContext* context, RenderingResources* resources)
{
	DX11VertexShader* shader = (DX11VertexShader*)resources->GetShaderByID(VERTEX_SHADER, DesiredState.m_Shader.GetState());

	if (shader)
		context->VSSetShader(shader->m_pShader, nullptr, 0);
	else
		context->VSSetShader(nullptr, nullptr, 0);
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the context buffers in the Desired State that need to change
void DX11VertexShaderStage::BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->VSSetConstantBuffers(
		DesiredState.m_ConstantBuffers.GetStartSlot(),
		DesiredState.m_ConstantBuffers.GetRange(),
		DesiredState.m_ConstantBuffers.GetFirstSlot());
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the sampler states in the Desired State that need to change
void DX11VertexShaderStage::BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->VSSetSamplers(
		DesiredState.m_SamplerStates.GetStartSlot(),
		DesiredState.m_SamplerStates.GetRange(),
		DesiredState.m_SamplerStates.GetFirstSlot());
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the shader resource views in the Desired State that need to change
void DX11VertexShaderStage::BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->VSSetShaderResources(
		DesiredState.m_ShaderResourceViews.GetStartSlot(),
		DesiredState.m_ShaderResourceViews.GetRange(),
		DesiredState.m_ShaderResourceViews.GetFirstSlot());
}

void DX11VertexShaderStage::BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	// Do nothing as Vertex Shaders don't support Unordered Access Views
}