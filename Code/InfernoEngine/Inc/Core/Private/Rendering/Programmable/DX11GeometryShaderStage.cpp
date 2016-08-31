#include "Rendering\Programmable\DX11GeometryShaderStage.h"
#include "Rendering\Shaders\DX11GeometryShader.h"
#include "Rendering\RenderingResources.h"

DX11GeometryShaderStage::DX11GeometryShaderStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11GeometryShaderStage::~DX11GeometryShaderStage()
{
}

// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the current shader set in Desired State if it needs to change
void DX11GeometryShaderStage::BindShader(ID3D11DeviceContext* context, RenderingResources* resources)
{
	DX11GeometryShader* shader = (DX11GeometryShader*)resources->GetShaderByID(GEOMETRY_SHADER, DesiredState.m_Shader.GetState());

	if (shader)
		context->GSSetShader(shader->m_pShader, nullptr, 0);
	else
		context->GSSetShader(nullptr, nullptr, 0);
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the context buffers in the Desired State that need to change
void DX11GeometryShaderStage::BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->GSSetConstantBuffers(
		DesiredState.m_ConstantBuffers.GetStartSlot(),
		DesiredState.m_ConstantBuffers.GetRange(),
		DesiredState.m_ConstantBuffers.GetFirstSlot());
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the sampler states in the Desired State that need to change
void DX11GeometryShaderStage::BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->GSSetSamplers(
		DesiredState.m_SamplerStates.GetStartSlot(),
		DesiredState.m_SamplerStates.GetRange(),
		DesiredState.m_SamplerStates.GetFirstSlot());
}
// IN: ID3D11DeviceContext* - pointer to the current context
// OUT: void
//
// Binds the shader resource views in the Desired State that need to change
void DX11GeometryShaderStage::BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->GSSetShaderResources(
		DesiredState.m_ShaderResourceViews.GetStartSlot(),
		DesiredState.m_ShaderResourceViews.GetRange(),
		DesiredState.m_ShaderResourceViews.GetFirstSlot());
}

void DX11GeometryShaderStage::BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	// Do nothing as Geometry shaders don't support Unordered Access Views
}