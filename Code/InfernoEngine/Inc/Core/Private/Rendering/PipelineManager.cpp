#include "Rendering\PipelineManager.h"

#include "Rendering\RenderingResources.h"

#include "Rendering\Shaders\DX11VertexShader.h"
#include "Rendering\Shaders\DX11GeometryShader.h"
#include "Rendering\Shaders\DX11PixelShader.h"

#include <d3d11.h>

PipelineManager::PipelineManager(ID3D11DeviceContext* context) : m_pContext(context)
{
	m_pShaders[(int)ShaderType::VERTEX_SHADER] = &m_VertexShader;
	m_pShaders[(int)ShaderType::GEOMETRY_SHADER] = &m_GeometryShader;
	m_pShaders[(int)ShaderType::PIXEL_SHADER] = &m_PixelShader;
	m_pShaders[(int)ShaderType::COMPUTE_SHADER] = &m_ComputeShader;
}

PipelineManager::~PipelineManager()
{
}

// InputAssembler Information
// --------------------------------------------------------------------

// IN: int          - ID of the vertex buffer
//     unsigned int - stride of buffer
//     unsigned int - offset of buffer
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetVertexBuffer(int ID, int slot, unsigned int stride, unsigned int offset)
{
	m_InputAssembler.DesiredState.m_VertexBuffers.SetState(ID, slot);
	m_InputAssembler.DesiredState.m_VertexBufferStrides.SetState(stride, slot);
	m_InputAssembler.DesiredState.m_VertexBufferOffsets.SetState(offset, slot);
}
// IN: int         - ID of the index buffer
//     DXGI_FORMAT - Format of the buffer
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetIndexBuffer(int ID, DXGI_FORMAT format)
{
	m_InputAssembler.DesiredState.m_IndexBuffer.SetState(ID);
	m_InputAssembler.DesiredState.m_IndexBufferFormat.SetState(format);
}
// IN: int - ID of InputLayout
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetInputLayout(int ID)
{
	m_InputAssembler.DesiredState.m_InputLayout.SetState(ID);
}
// IN: D3D11_PRIMITIVE_TOPOLOGY - Topology to use
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_InputAssembler.DesiredState.m_PrimitiveTopology.SetState(topology);
}

// RasterizerState Information
// --------------------------------------------------------------------

// IN: int - ID of Rasterizer State
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetRaterizerState(int ID)
{
	m_RasterizerState.DesiredState.m_RasterizerState.SetState(ID);
}
// IN: int  - number of views
//     int* - array of views to apply
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetViewport(int count, int* IDs)
{
	m_RasterizerState.DesiredState.m_ViewportCount.SetState(count);

	for (int index = 0; index < count; index++)
		m_RasterizerState.DesiredState.m_Viewports.SetState(IDs[index], index);
}

// OutputMerger Information
// --------------------------------------------------------------------

// IN: int - ID of Blend State
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetBlendState(int ID)
{
	m_OutputMerger.DesiredState.m_BlendState.SetState(ID);
}
// IN: int          - ID of Depth Stencil State
//     unsigned int - Refrence to use against the state
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetDepthStencilState(int ID, unsigned int ref)
{
	m_OutputMerger.DesiredState.m_DepthStencilState.SetState(ID);
	m_OutputMerger.DesiredState.m_DepthStencilRef.SetState(ref);
}
// IN: int  - number of views
//     int* - array of views to apply
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetRenderTargetViews(int count, int* IDs)
{
	for (int index = 0; index < count; index++)
		m_OutputMerger.DesiredState.m_RenderTargetViews.SetState(IDs[index], index);
}
// IN: int - ID of Depth Stencil View
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetDepthStencilView(int ID)
{
	m_OutputMerger.DesiredState.m_DepthStencilView.SetState(ID);
}

// Shader Information
// --------------------------------------------------------------------

// IN: int - ID of Vertex Shader
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetVertexShader(int ID, RenderingResources* resources)
{
	m_VertexShader.DesiredState.m_Shader.SetState(ID);

	DX11Shader* shader = resources->GetShaderByID(VERTEX_SHADER, ID);

	if (shader)
	{
		for (size_t i = 0; i < shader->m_vConstantBuffers.size(); i++)
			m_VertexShader.DesiredState.m_ConstantBuffers.SetState(shader->m_vConstantBuffers[i]->m_pBuffer, shader->m_vBufferSlots[i]);
	}
}
// IN: int - ID of Geometry Shader
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetGeometryShader(int ID, RenderingResources* resources)
{
	m_GeometryShader.DesiredState.m_Shader.SetState(ID);

	DX11Shader* shader = resources->GetShaderByID(GEOMETRY_SHADER, ID);

	if (shader)
	{
		for (size_t i = 0; i < shader->m_vConstantBuffers.size(); i++)
			m_GeometryShader.DesiredState.m_ConstantBuffers.SetState(shader->m_vConstantBuffers[i]->m_pBuffer, shader->m_vBufferSlots[i]);
	}
}
// IN: int - ID of Pixel Shader
// OUT: void
//
// Updates the desired state with incoming information
void PipelineManager::SetPixelShader(int ID, RenderingResources* resources)
{
	m_PixelShader.DesiredState.m_Shader.SetState(ID);

	DX11Shader* shader = resources->GetShaderByID(PIXEL_SHADER, ID);

	if (shader)
	{
		for (size_t i = 0; i < shader->m_vConstantBuffers.size(); i++)
			m_PixelShader.DesiredState.m_ConstantBuffers.SetState(shader->m_vConstantBuffers[i]->m_pBuffer, shader->m_vBufferSlots[i]);
	}
}
// IN: int - ID of the Compute Shader
// OUT: void
//
// Updates the desired state with the incoming information
void PipelineManager::SetComputeShader(int ID, RenderingResources* resources)
{
	m_ComputeShader.DesiredState.m_Shader.SetState(ID);

	DX11Shader* shader = resources->GetShaderByID(COMPUTE_SHADER, ID);

	if (shader)
	{
		for (size_t i = 0; i < shader->m_vConstantBuffers.size(); i++)
			m_ComputeShader.DesiredState.m_ConstantBuffers.SetState(shader->m_vConstantBuffers[i]->m_pBuffer, shader->m_vBufferSlots[i]);
	}
}

// IN: ShaderType   - Type of Shader to apply to
//     unsigned int - ID of the constant buffer
//     unsigned int - Slot of the buffer
// OUT: void
//
// Assigns the passed in constant buffer to the pipeline
void PipelineManager::SetConstantBuffer(ShaderType type, unsigned int ID, unsigned int Slot, RenderingResources* resources)
{
	DX11Buffer* buffer = resources->GetConstantBufferByID(ID);

	if (buffer && buffer->GetType() == CONSTANT_BUFFER)
		m_pShaders[type]->DesiredState.m_ConstantBuffers.SetState(buffer->m_pBuffer, Slot);
}

// IN: ShaderType         - type to apply sampler to
//     int                - ID of sampler
//     int                - slot to attach
//     RenderingResource* - pointer to the resources
// OUT: void
//
// Applies a sampler state to the shader type
void PipelineManager::SetSamplerState(ShaderType type, int ID, int slot, RenderingResources* resouces)
{
	DX11SampleState* SS = resouces->GetSamplerStateByID(ID);
	ID3D11SamplerState* d3dSS = nullptr;

	if (SS)
		d3dSS = SS->m_pSamplerState;

	m_pShaders[type]->DesiredState.m_SamplerStates.SetState(d3dSS, slot);
}

// IN: ShaderType         - type to apply sampler to
//     int                - ID of image
//     int                - slot to attach
//     RenderingResource* - pointer to the resources
// OUT: void
//
// Applies a image to the shader type requested
bool PipelineManager::SetShaderResourceView(ShaderType type, int ID, int slot, RenderingResources* resources)
{
	DX11ShaderResourceView* srv = resources->GetImageByID(ID);
	ID3D11ShaderResourceView* iSrv = nullptr;

	if (srv)
		iSrv = srv->m_pSRV;

	m_pShaders[type]->DesiredState.m_ShaderResourceViews.SetState(iSrv, slot);
	
	return true;
}

// IN: ShaderType         - type to apply the UAV to
//     int                - ID of the UAV
//     int                - Slot to attach
//     RenderingResoucre* - pointer to the resources
// OUT: void
//
// Applies the requested UAV to the requested shader
void PipelineManager::SetUnorderedAccessView(ShaderType type, int ID, int slot, RenderingResources* resources, int Count)
{
	DX11UnorderedAccessView* uav = resources->GetUnorderedAccessViewByID(ID);
	ID3D11UnorderedAccessView* pUAV = nullptr;

	if (uav)
		pUAV = uav->m_pUAV;

	m_pShaders[type]->DesiredState.m_UnorderedAccessViews.SetState(pUAV, slot);
	m_pShaders[type]->DesiredState.m_UAVInitialCounts.SetState(Count, slot);
}

// IN: int  - ID of the pixel shader
//     int  - ID of the rasterizer state
//     int  - ID of the blend state
//     int* - List of SamplerState IDs
//     int  - Count of the sampler states
// OUT: int - ID of the added effect
//
// Creates an effect and adds it to the list of effects if it doesn't already exits and returns the ID
int PipelineManager::CreateEffect(const char* name, int pixelShader, int rasterizerState, int blendState, int* samplerStates, int samplerCount)
{
	for (size_t i = 0; i < m_vEffects.size(); i++)
	{
		if (m_vEffects[i].m_sName == name)
			return (int)i;
	}

	Effect newEffect;
	newEffect.m_sName = name;
	newEffect.SetPixelShader(pixelShader);
	newEffect.SetRasterizerState(rasterizerState);
	newEffect.SetBlendState(blendState);

	for (int i = 0; i < samplerCount; i++)
		newEffect.AddSamplerState(samplerStates[i]);

	m_vEffects.push_back(newEffect);
	return (int)m_vEffects.size() - 1;
}

// IN: char* - name of the effect
// OUT: int - ID of the effect
//
// Returns the ID for the effect of the corresonding name if it exists
int PipelineManager::GetEffectByName(const char* name) const
{
	for (size_t i = 0; i < m_vEffects.size(); i++)
	{
		if (m_vEffects[i].m_sName == name)
			return (int)i;
	}

	return -1;
}
// IN: int - ID of the effect
// OUT: Effect* - Pointer to the effect, nullptr on error
//
// Returns the Effect based on the passed in ID
const Effect* PipelineManager::GetEffectByID(const int ID) const
{
	if (ID < 0 || ID >= m_vEffects.size())
		return nullptr;

	return &m_vEffects[ID];
}

// IN: color[4]           - color to clear to
//     ReneringResources* - pointer to the resources
// OUT: void
//
// Clears all the active render targets and depth stencil views
void PipelineManager::ClearRenderTargetViews(float color[4], RenderingResources* resources)
{
	for (unsigned int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		DX11RenderTargetView* rtv = resources->GetRenderTargetViewByID(m_OutputMerger.GetCurrentState().m_RenderTargetViews.GetState(i));

		if (rtv)
			m_pContext->ClearRenderTargetView(rtv->m_pRTV, color);
	}
}

// IN: int - ID of the Depth Stencil View
// OUT: void
//
// Clears the Depth Stencil View Requested
void PipelineManager::ClearDepthStencilView(int ID, float depth, RenderingResources* resources)
{
	DX11DepthStencilView* dsv = resources->GetDepthStencilViewByID(ID);

	if (dsv)
		m_pContext->ClearDepthStencilView(dsv->m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
}

// IN: void
// OUT: void
//
// Clears the update flags
void PipelineManager::ClearStates()
{
	m_InputAssembler.ClearCurrentState();
	m_InputAssembler.ClearDesiredState();

	m_VertexShader.ClearCurrentState();
	m_VertexShader.ClearDesiredState();
	
	m_GeometryShader.ClearCurrentState();
	m_GeometryShader.ClearDesiredState();
	
	m_RasterizerState.ClearCurrentState();
	m_RasterizerState.ClearDesiredState();

	m_PixelShader.ClearCurrentState();
	m_PixelShader.ClearDesiredState();
	
	m_OutputMerger.ClearCurrentState();
	m_OutputMerger.ClearDesiredState();

	m_ComputeShader.ClearCurrentState();
	m_ComputeShader.ClearDesiredState();
}

// IN: void
// OUT: void
//
// Applies all the desired states to prepare for drawing
void PipelineManager::ApplyDesiredStates(RenderingResources* resources)
{
	m_InputAssembler.ApplyDesiredState(m_pContext, resources);
	m_RasterizerState.ApplyDesiredState(m_pContext, resources);
	m_OutputMerger.ApplyDesiredState(m_pContext, resources);

	m_VertexShader.ApplyDesiredState(m_pContext, resources);
	m_GeometryShader.ApplyDesiredState(m_pContext, resources);
	m_PixelShader.ApplyDesiredState(m_pContext, resources);
	m_ComputeShader.ApplyDesiredState(m_pContext, resources);
}

// IN: int                 - ID of the effect to use
//     int                 - Starting index to draw from
//     int                 - Number of indices to draw
//     RenderingResources* - Pointer to the resources
// OUT: void
//
// Draws the passed in mesh using the effect requested
void PipelineManager::DrawIndexed(int EffectID, int startIndex, int numIndices, RenderingResources* resources)
{
	if (m_vEffects.size() == 0)
	{
		ALERT("No effects are available for drawling! Please create a new effect before attempting to draw!");
		return;
	}

	int eID = EffectID;

	if (eID < 0 || eID >= m_vEffects.size())
		eID = 0;

	Effect *currEffect = &m_vEffects[eID];

	m_RasterizerState.DesiredState.m_RasterizerState.SetState(currEffect->GetRasterizerStateID());
	SetPixelShader(currEffect->GetPixelShaderID(), resources);
	m_OutputMerger.DesiredState.m_BlendState.SetState(currEffect->GetBlendStateID());

	auto samplers = currEffect->GetSamplerStateIDs();
	for (size_t i = 0; i < samplers.size(); i++)
	{
		DX11SampleState* sampler = resources->GetSamplerStateByID(samplers[i]);

		if (sampler)
			m_PixelShader.DesiredState.m_SamplerStates.SetState(sampler->m_pSamplerState, (unsigned int)i);
		else
			m_PixelShader.DesiredState.m_SamplerStates.SetState(nullptr, (unsigned int)i);
	}

	ApplyDesiredStates(resources);
	m_pContext->DrawIndexed(numIndices, startIndex, 0);
}

// IN: int   - ID of the effect to use
//     Mesh* - Pointer to the mesh to draw
// OUT: void
//
// Draws the passed in mesh using the effect requested
void PipelineManager::DrawIndexed(int EffectID, SubMesh* Mesh, RenderingResources* resources)
{
	if (m_vEffects.size() == 0)
	{
		ALERT("No effects are available for drawling! Please create a new effect before attempting to draw!");
		return;
	}

	int eID = EffectID;

	if (eID < 0 || eID >= m_vEffects.size())
		eID = 0;

	Effect *currEffect = &m_vEffects[eID];

	m_RasterizerState.DesiredState.m_RasterizerState.SetState(currEffect->GetRasterizerStateID());
	SetPixelShader(currEffect->GetPixelShaderID(), resources);
	m_OutputMerger.DesiredState.m_BlendState.SetState(currEffect->GetBlendStateID());

	auto samplers = currEffect->GetSamplerStateIDs();
	for (size_t i = 0; i < samplers.size(); i++)
		m_PixelShader.DesiredState.m_SamplerStates.SetState(resources->GetSamplerStateByID(samplers[i])->m_pSamplerState, (unsigned int)i);

	ApplyDesiredStates(resources);
	m_pContext->DrawIndexed(Mesh->indiceCount, Mesh->startIndex, 0);
}

// IN: int          - ID of the effect to use
//     unsigned int - start index
//     unsinged int - num indices
//     unsigned int - start vertex
// OUT: void
//
// Draws the passed in mesh using the effect requested
void PipelineManager::DrawIndexed(int EffectID, unsigned int startIndex, unsigned int numIndices, unsigned int startVertex, RenderingResources* resources)
{
	if (m_vEffects.size() == 0)
	{
		ALERT("No effects are available for drawling! Please create a new effect before attempting to draw!");
		return;
	}

	int eID = EffectID;

	if (eID < 0 || eID >= m_vEffects.size())
		eID = 0;

	Effect *currEffect = &m_vEffects[eID];

	m_RasterizerState.DesiredState.m_RasterizerState.SetState(currEffect->GetRasterizerStateID());
	SetPixelShader(currEffect->GetPixelShaderID(), resources);
	m_OutputMerger.DesiredState.m_BlendState.SetState(currEffect->GetBlendStateID());

	auto samplers = currEffect->GetSamplerStateIDs();
	for (size_t i = 0; i < samplers.size(); i++)
	{
		DX11SampleState* sampler = resources->GetSamplerStateByID(samplers[i]);
		
		if (sampler)
			m_PixelShader.DesiredState.m_SamplerStates.SetState(sampler->m_pSamplerState, (unsigned int)i);
	}

	ApplyDesiredStates(resources);
	m_pContext->DrawIndexed(numIndices, startIndex, startVertex);
}

void PipelineManager::Draw(int EffectID, int StartVertex, int NumVerts, RenderingResources* resources)
{
	if (m_vEffects.size() == 0)
	{
		ALERT("No effects are available for drawling! Please create a new effect before attempting to draw!");
		return;
	}

	int eID = EffectID;

	if (eID < 0 || eID >= m_vEffects.size())
		eID = 0;

	Effect *currEffect = &m_vEffects[eID];

	m_RasterizerState.DesiredState.m_RasterizerState.SetState(currEffect->GetRasterizerStateID());
	SetPixelShader(currEffect->GetPixelShaderID(), resources);
	m_OutputMerger.DesiredState.m_BlendState.SetState(currEffect->GetBlendStateID());

	auto samplers = currEffect->GetSamplerStateIDs();
	for (size_t i = 0; i < samplers.size(); i++)
		m_PixelShader.DesiredState.m_SamplerStates.SetState(resources->GetSamplerStateByID(samplers[i])->m_pSamplerState, (unsigned int)i);

	ApplyDesiredStates(resources);
	m_pContext->Draw(NumVerts, StartVertex);
}

// IN: int  - ID of the buffer with the draw arguments
//     UINT - offset in bytes for the draw arguments
// OUT: void
//
// Applies changes to the pipeline and uses the buffer from the passed in ID to draw
void PipelineManager::DrawIndirect(int BufferID, UINT offset, RenderingResources* resources)
{
	DX11Buffer* BufferArgs = resources->GetBufferByID(BufferID);
	
	if (!BufferArgs)
		return;

	ID3D11Buffer* buffer = BufferArgs->m_pBuffer;

	ApplyDesiredStates(resources);
	m_pContext->DrawInstancedIndirect(buffer, offset);
}

// IN: unsigned int - X group threads
//     unsigned int - Y group threads
//     unsigned int - Z group threads
// OUT: void
//
// Calls dispatch on the current pipeline
void PipelineManager::Dispatch(UINT x, UINT y, UINT z, RenderingResources* resources)
{
	ApplyDesiredStates(resources);
	m_pContext->Dispatch(x, y, z);
}