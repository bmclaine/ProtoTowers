#include "Rendering\Programmable\DX11ComputeShaderStage.h"
#include "Rendering\Shaders\DX11ComputeShader.h"
#include "Rendering\RenderingResources.h"

DX11ComputeShaderStage::DX11ComputeShaderStage()
{
	DesiredState.SetPreviousState(&CurrentState);
}

DX11ComputeShaderStage::~DX11ComputeShaderStage()
{
}

void DX11ComputeShaderStage::BindShader(ID3D11DeviceContext* context, RenderingResources* resources)
{
	DX11ComputeShader* shader = (DX11ComputeShader*)resources->GetShaderByID(COMPUTE_SHADER, DesiredState.m_Shader.GetState());

	if (shader)
		context->CSSetShader(shader->m_pShader, nullptr, 0);
	else
		context->CSSetShader(nullptr, nullptr, 0);
}
void DX11ComputeShaderStage::BindConstantBuffer(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->CSSetConstantBuffers(
		DesiredState.m_ConstantBuffers.GetStartSlot(),
		DesiredState.m_ConstantBuffers.GetRange(),
		DesiredState.m_ConstantBuffers.GetFirstSlot());
}
void DX11ComputeShaderStage::BindSamplerState(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->CSSetSamplers(
		DesiredState.m_SamplerStates.GetStartSlot(),
		DesiredState.m_SamplerStates.GetRange(),
		DesiredState.m_SamplerStates.GetFirstSlot());
}
void DX11ComputeShaderStage::BindShaderResourceViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	context->CSSetShaderResources(
		DesiredState.m_ShaderResourceViews.GetStartSlot(),
		DesiredState.m_ShaderResourceViews.GetRange(),
		DesiredState.m_ShaderResourceViews.GetFirstSlot());
}
void DX11ComputeShaderStage::BindUnorderedAccessViews(ID3D11DeviceContext* context, RenderingResources* resources)
{
	unsigned int minStartSlot = min(DesiredState.m_UnorderedAccessViews.GetStartSlot(), DesiredState.m_UAVInitialCounts.GetStartSlot());
	unsigned int maxEndSlot = max(DesiredState.m_UnorderedAccessViews.GetEndSlot(), DesiredState.m_UAVInitialCounts.GetEndSlot());

	context->CSSetUnorderedAccessViews(
		minStartSlot,
		maxEndSlot - minStartSlot + 1,
		DesiredState.m_UnorderedAccessViews.GetSlotLocation(minStartSlot),
		DesiredState.m_UAVInitialCounts.GetSlotLocation(minStartSlot));
}