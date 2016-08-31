#pragma once

#include "ShaderInfo.h"

#include "Fixed Stages\DX11InputAssemblerStage.h"
#include "Fixed Stages\DX11OutputMergerStage.h"
#include "Fixed Stages\DX11RasterizerStateStage.h"

#include "Programmable\DX11VertexShaderStage.h"
#include "Programmable\DX11GeometryShaderStage.h"
#include "Programmable\DX11PixelShaderStage.h"
#include "Programmable\DX11ComputeShaderStage.h"

#include "Assets\Mesh.h"
#include "Rendering\Effect.h"

struct ID3D11DeviceContext;
class RenderingResources;

class PipelineManager
{
private:
	DX11InputAssemblerStage m_InputAssembler;
	DX11RasterizerStateStage m_RasterizerState;
	DX11OutputMergerStage m_OutputMerger;

	DX11VertexShaderStage m_VertexShader;
	DX11GeometryShaderStage m_GeometryShader;
	DX11PixelShaderStage m_PixelShader;
	DX11ComputeShaderStage m_ComputeShader;
	DX11Stage* m_pShaders[4];

	std::vector<Effect> m_vEffects;

	ID3D11DeviceContext* m_pContext;

public:
	PipelineManager(ID3D11DeviceContext* context);
	~PipelineManager();

	// InputAssembler Information
	// --------------------------------------------------------------------

	// IN: int          - ID of the vertex buffer
	//     unsigned int - stride of buffer
	//     unsigned int - offset of buffer
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetVertexBuffer(int ID, int slot, unsigned int stride, unsigned int offset);
	// IN: int         - ID of the index buffer
	//     DXGI_FORMAT - Format of the buffer
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetIndexBuffer(int ID, DXGI_FORMAT format);
	// IN: int - ID of InputLayout
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetInputLayout(int ID);
	// IN: D3D11_PRIMITIVE_TOPOLOGY - Topology to use
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	// RasterizerState Information
	// --------------------------------------------------------------------

	// IN: int - ID of Rasterizer State
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetRaterizerState(int ID);
	// IN: int  - number of views
	//     int* - array of views to apply
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetViewport(int count, int* IDs);

	// OutputMerger Information
	// --------------------------------------------------------------------

	// IN: int - ID of Blend State
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetBlendState(int ID);
	// IN: int          - ID of Depth Stencil State
	//     unsigned int - Refrence to use against the state
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetDepthStencilState(int ID, unsigned int ref);
	// IN: int  - number of views
	//     int* - array of views to apply
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetRenderTargetViews(int count, int* IDs);
	// IN: int - ID of Depth Stencil View
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetDepthStencilView(int ID);

	// Shader Information
	// --------------------------------------------------------------------

	// IN: int - ID of Vertex Shader
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetVertexShader(int ID, RenderingResources* resources);
	// IN: int - ID of Geometry Shader
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetGeometryShader(int ID, RenderingResources* resources);
	// IN: int - ID of Pixel Shader
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetPixelShader(int ID, RenderingResources* resources);
	// IN: int - ID of the Compute Shader
	// OUT: void
	//
	// Updates the desired state with the incoming information
	void SetComputeShader(int ID, RenderingResources* resources);

	// IN: ShaderType   - Type of Shader to apply to
	//     unsigned int - ID of the constant buffer
	//     unsigned int - Slot of the buffer
	// OUT: void
	//
	// Assigns the passed in constant buffer to the pipeline
	void SetConstantBuffer(ShaderType type, unsigned int ID, unsigned int Slot, RenderingResources* resources);

	// IN: ShaderType         - type to apply sampler to
	//     int                - ID of sampler
	//     int                - slot to attach
	//     RenderingResource* - pointer to the resources
	// OUT: void
	//
	// Applies a sampler state to the shader type
	void SetSamplerState(ShaderType type, int ID, int slot, RenderingResources* resouces);

	// IN: ShaderType         - type to apply sampler to
	//     int                - ID of image
	//     int                - slot to attach
	//     RenderingResource* - pointer to the resources
	// OUT: void
	//
	// Applies a image to the shader type requested
	bool SetShaderResourceView(ShaderType type, int ID, int slot, RenderingResources* resources);

	// IN: ShaderType         - type to apply the UAV to
	//     int                - ID of the UAV
	//     int                - Slot to attach
	//     RenderingResoucre* - pointer to the resources
	// OUT: void
	//
	// Applies the requested UAV to the requested shader
	void SetUnorderedAccessView(ShaderType type, int ID, int slot, RenderingResources* resources, int Count = -1);

	// IN: char* - Name of the effect
	//     int   - ID of the pixel shader
	//     int   - ID of the rasterizer state
	//     int   - ID of the blend state
	//     int*  - List of SamplerState IDs
	//     int   - Count of the sampler states
	// OUT: int - ID of the added effect
	//
	// Creates an effect and adds it to the list of effects if it doesn't already exits and returns the ID
	int CreateEffect(const char* name, int pixelShader, int rasterizerState, int blendState, int* samplerStates, int samplerCount);

	// IN: char* - name of the effect
	// OUT: int - ID of the effect
	//
	// Returns the ID for the effect of the corresonding name if it exists
	int GetEffectByName(const char* name) const;
	// IN: int - ID of the effect
	// OUT: Effect* - Pointer to the effect, nullptr on error
	//
	// Returns the Effect based on the passed in ID
	const Effect* GetEffectByID(const int ID) const;

	// IN: color[4]           - color to clear to
	//     ReneringResources* - pointer to the resources
	// OUT: void
	//
	// Clears all the active render targets and depth stencil views
	void ClearRenderTargetViews(float color[4], RenderingResources* resources);

	// IN: int - ID of the Depth Stencil View
	// OUT: void
	//
	// Clears the Depth Stencil View Requested
	void ClearDepthStencilView(int ID, float depth, RenderingResources* resources);

	// IN: void
	// OUT: void
	//
	// Clears all the states
	void ClearStates();

	// IN: void
	// OUT: void
	//
	// Applies all the desired states to prepare for drawing
	void ApplyDesiredStates(RenderingResources* resources);

	// IN: int                 - ID of the effect to use
	//     int                 - Starting index to draw from
	//     int                 - Number of indices to draw
	//     RenderingResources* - Pointer to the resources
	// OUT: void
	//
	// Draws the passed in mesh using the effect requested
	void DrawIndexed(int EffectID, int startIndex, int numIndices, RenderingResources* resources);

	// IN: int   - ID of the effect to use
	//     Mesh* - Pointer to the mesh to draw
	// OUT: void
	//
	// Draws the passed in mesh using the effect requested
	void DrawIndexed(int EffectID, SubMesh* Mesh, RenderingResources* resources);

	// IN: int          - ID of the effect to use
	//     unsigned int - start index
	//     unsinged int - num indices
	//     unsigned int - start vertex
	// OUT: void
	//
	// Draws the passed in mesh using the effect requested
	void DrawIndexed(int EffectID, unsigned int startIndex, unsigned int numIndices, unsigned int startVertex, RenderingResources* resources);

	// IN: int - ID of the effect to use
	//     int - Start Vertex
	//     int - Num Vertices
	// OUT: void
	//
	// Calls a standard draw call using the passed in effect ID
	void Draw(int EffectID, int StartVertex, int NumVerts, RenderingResources* resources);

	// IN: int  - ID of the buffer with the draw arguments
	//     UINT - offset in bytes for the draw arguments
	// OUT: void
	//
	// Applies changes to the pipeline and uses the buffer from the passed in ID to draw
	void DrawIndirect(int BufferID, UINT offset, RenderingResources* resources);

	// IN: unsigned int       - X group threads
	//     unsigned int       - Y group threads
	//     unsigned int       - Z group threads
	//     RenderingResource* - pointer to the resources
	// OUT: void
	//
	// Calls dispatch on the current pipeline
	void Dispatch(UINT x, UINT y, UINT z, RenderingResources* resources);
};