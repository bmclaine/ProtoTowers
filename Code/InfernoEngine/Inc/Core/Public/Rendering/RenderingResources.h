// Author: Jonthan Gyurkovics
//
// The purpose of this class is to hold and manage any resources that will need to be spanned across rendering components that belong
// to the graphics manager. This class is responsible for creating and deleting these resources.

#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <vector>

#include "Shaders\DX11Shader.h"
#include "Shaders\ShaderReflection.h"

#include "Buffers\DX11Buffer.h"

#include "States\DX11InputLayout.h"
#include "States\DX11DepthStencilView.h"
#include "States\DX11RenderTargetView.h"
#include "States\DX11ShaderResourceView.h"
#include "States\DX11UnorderedAccessView.h"
#include "States\DX11BlendState.h"
#include "States\DX11SampleState.h"
#include "States\DX11RasterizerState.h"
#include "States\DX11DepthStencilState.h"

#if defined(_DEBUG) || defined(DEBUG)
#define SET_DEBUG_NAME(resource, name) resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#else
#define SET_DEBUG_NAME(resource, name)
#endif

class RenderingResources
{
private:
	std::vector<DX11Shader*> m_vShaders;						// Expanding array that holds all the known shaders
	std::vector<DX11Buffer*> m_vBuffers;						// Expanding array that holds all the known buffers
	std::vector<DX11InputLayout*> m_vInputLayouts;				// Expanding array that holds all the known input layouts
	std::vector<DX11ShaderResourceView*> m_vImages;				// Expanding array that holds all the known shader resource views
	std::vector<DX11UnorderedAccessView*> m_vUAVs;				// Expanding array that holds all the known unordered access views
	std::vector<DX11RenderTargetView*> m_vRenderTargets;		// Expanding array that holds all the known render targets
	std::vector<DX11DepthStencilView*> m_vDepthStecilViews;		// Expanding array that holds all the known depth stencil views
	std::vector<DX11SampleState*> m_vSampleStates;				// Expanding array that holds all the known sample states
	std::vector<DX11RasterizerState*> m_vRasterizerStates;		// Expanding array that holds all the known rasterizer states
	std::vector<DX11BlendState*> m_vBlendStates;				// Expanding array that holds all the known blend states
	std::vector<DX11DepthStencilState*> m_vDpethStencilStates;	// Expanding array that holds all the known depth stencil states
	std::vector<D3D11_VIEWPORT>	m_vViewports;					// Expanding array that holds all the known viewports

	ID3D11Device* m_pDevice;									// Pointer to the device
	ID3D11DeviceContext* m_pContext;							// Pointer to the context

	// IN: wchar_t* - name of the shader
	// OUT: DX11Shader* - pointer to the shader with the corresponding name
	//
	// Returns the first shader with the corresponding name if one exists
	DX11Shader* GetShaderByName(const wchar_t* shaderName);

	// IN: void* - pointer to the new anisotropic value
	// OUT: void
	//
	// Function to be called by the event system when the anisotropic value is changed
	void AnisotropicChange(void* pData);

public:
	RenderingResources();
	~RenderingResources();

	// IN: ID3D11Device* - pointer to the device
	// OUT: void
	//
	// Stores a local pointer to the device
	void SetDevice(ID3D11Device* device);
	// IN: ID3D11DeviceContext* - pointer to the context
	// OUT: void
	//
	// Stores a local pointer to the device context
	void SetContext(ID3D11DeviceContext* context);

	// IN: ShaderType - the type of shader being loaded
	//     wchar_t*   - the name of the shader
	// OUT: int - ID of loaded shader, -1 for error
	//
	// Loads the shader if it doesn't already exist, otherwise returns the ID of the shader
	int LoadShader(ShaderType type, const wchar_t* shaderName);

	// IN: ShaderType - the type of shader being loaded
	//     wchar_t*   - the name of the shader
	//     wchar_t*   - the entry function name
	//     wchar_t*   - the model name, ex. vs_4_0
	// OUT: int - ID of loaded shader, -1 for error
	//
	// Loads the shader if it doesn't already exist, otherwise returns the ID of the shader
	int LoadUncompiledShader(ShaderType type, const wchar_t* shaderName, const wchar_t* function, const wchar_t* model);

	void CreateGeometryShaderWithStreamOutput(int ID, D3D11_SO_DECLARATION_ENTRY* entry, UINT numEntries, UINT* strides, UINT numStrides);

	// IN: void
	// OUT: bool - status of applying constant buffers
	//
	// Loops through all our shaders and applies any constant buffers they need based on reflection
	bool ApplyConstantBuffers();

	// IN: wstring            - name of the buffer
	//     D3D11_BUFFER_DESC* - pointer to the descriptor
	// OUT: int - ID of the buffer, -1 on error
	//
	// Creates a Vertex Buffer based on the incoming information
	int CreateVertexBuffer(std::wstring bufferName, D3D11_BUFFER_DESC* desc);

	// IN: wstring            - name of the buffer
	//     D3D11_BUFFER_DESC* - pointer to the descriptor
	// OUT: int - ID of the buffer, -1 on error
	//
	// Creates an Index Buffer based on the incoming information
	int CreateIndexBuffer(std::wstring bufferName, D3D11_BUFFER_DESC* desc);

	// IN: UINT - size of the constant buffer
	// OUT: int - ID of the buffer, -1 for error
	//
	// Creates a new constant buffer if one of the same name doesn't exist, otherwise returns the buffer ID if it does exist
	int CreateConstantBuffer(std::wstring bufferName, UINT bufferSize);

	// IN: wstring            - name of the buffer
	//     D3D11_BUFFER_DESC* - pointer to the descriptor
	// OUT: int - ID of the buffer, -1 on error
	//
	// Creates a Structured Buffer based on the incoming information
	int CreateStructuredBuffer(std::wstring bufferName, D3D11_BUFFER_DESC* desc);

	// IN: D3D11_INPUT_ELEMENT_DESC* - pointer to the elements list
	//     UINT                      - number of elements
	//     UINT                      - ID of shader with input signature
	// OUT: int - ID of the created layout, -1 on error
	//
	// Creates a layout and returns the ID
	int CreateInputLayout(std::wstring name, D3D11_INPUT_ELEMENT_DESC* elems, UINT elemCount, UINT ShaderID);

	// IN: string - name of the image to load including the type (ex. png, bmp, etc..)
	// OUT: int - ID of the image, -1 on error
	//
	// Creates an image based on incoming name and returns the ID
	int CreateImage(std::string imageName);
	// IN: wstring - name of the image to load including the type (ex. png, bmp, etc..)
	// OUT: int - ID of the image, -1 on error
	//
	// Creates an image based on incoming name and returns the ID
	int CreateImage(std::wstring imageName);
	// IN: wstring - name of the image
	// OUT: int - ID of the slot
	//
	// Creates an empty texture slot for later use
	int RequestTextureSlot(std::wstring imageName);
	// IN: UINT   - ID of the slot the data will go into
	//     void*  - Pointer to the memory location of the image data
	//     size_t - Size of the data in bytes
	// OUT: void
	//
	// Create an image based on the data stored in memory and store it in the slot requested
	void CreateImageFromMemory(UINT imageID, void* image, size_t sizeInBytes);
	// IN: wstring - name of the image
	//     void*   - pointer to a Shader Resource View
	// OUT: int - ID of the Image added
	//
	// Adds the shader resource view to the active list of images
	int AddImage(std::wstring name, void* image);
	// IN: wstring  - Name for the ShaderResource
	//     wstring* - Pointer to the array of image names
	//     uint     - number of image names
	// OUT: int - ID to the ShaderResourceView
	int CreateTextureArray(std::wstring name, std::wstring* textureNames, UINT numTextures);

	// IN: int - ID of the buffer to create a Shader Resource for
	// OUT: int - ID of the Shader Resource, -1 on error
	//
	// Creates a Shader Resource for the requested buffer
	int CreateBufferSRV(int Buffer, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_DESC *desc = nullptr);

	// IN: int - ID of the buffer to create a Unordered Access View
	// OUT: int - ID of the Unordered Access View
	//
	// Creates an unordered access view for the requested buffer and names it the same name as the buffer
	int CreateUnorderedAccessView(int BufferID, UINT Flags = 0, DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_DESC *desc = nullptr);
	// IN: int                  - ID of the shader resource view
	//     UINT                 - Size of each element in bytes
	//     UINT                 - Flags to apply to the UAV
	// OUT: int - ID of the created UAV, -1 on error
	//
	// Creates an unordered access view for the requested shader resource view
	int CreateUnorderedAccessViewFromTexture(int textureID, UINT Flags, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	// IN: ID3D11Texture2D*               - pointer to the texture to use
	//     D3D11_RENDER_TARGET_VIEW_DESC* - pointer to the descriptor
	// OUT: int - ID of the RTV, -1 on error
	//
	// Creates a Render Target View based on the incoming information and returns the ID
	int CreateRenderTargetView(std::wstring name, ID3D11Texture2D* texture, D3D11_RENDER_TARGET_VIEW_DESC* desc);
	// IN: UINT                           - ID of image to use
	//     D3D11_RENDER_TARGET_VIEW_DESC* - pointer to the descriptor
	// OUT: int - ID of RTV, -1 on error
	//
	// Creates a Render Target View based on the incoming information and returns the ID
	int CreateRenderTargetView(std::wstring name, UINT imageID, D3D11_RENDER_TARGET_VIEW_DESC* desc);

	// IN: ID3D11Texture2D*               - pointer to the texture to use
	//     D3D11_DEPTH_STENCIL_VIEW_DESC* - pointer to the descriptor
	// OUT: int - ID of DSV, -1 on error
	//
	// Creates a Depth Stencil View based on the incoming information
	int CreateDepthStencilView(std::wstring name, ID3D11Texture2D* texture, D3D11_DEPTH_STENCIL_VIEW_DESC* desc);
	// IN: UINT                           - ID of image to use
	//     D3D11_DEPTH_STENCIL_VIEW_DESC* - pointer to the descriptor
	// OUT: int - ID of DSV, -1 on error
	//
	// Creates a Depth Stencil View based on the incoming information
	int CreateDepthStencilView(std::wstring name, UINT imageID, D3D11_DEPTH_STENCIL_VIEW_DESC* desc);

	// IN: D3D11_SAMPLER_DESC* - pointer to the descriptor
	// OUT: int - ID of the Sampler State, -1 on error
	//
	// Creates a sampler state base on the incoming information
	int CreateSamplerState(std::wstring name, D3D11_SAMPLER_DESC* desc);

	// IN: wstring                - name of the sampler state
	//     D3D11_RASTERIZER_DESC* - pointer to the descriptor
	// OUT: int - ID of the Rasterizer State, -1 on error
	//
	// Creates a rasterizer state based on the incoming information
	int CreateRasterizerState(std::wstring name, D3D11_RASTERIZER_DESC* desc);

	// IN: D3D11_BLEND_DESC* - pointer to the descriptor
	// OUT: int - ID of the Blend State, -1 on error
	//
	// Creates a blend state base on the incoming information
	int CreateBlendState(std::wstring name, D3D11_BLEND_DESC* desc);

	// IN: D3D11_DEPTH_STENCIL_DESC* - pointer to the descriptor
	// OUT: int - ID of the Depth Stencil State, -1 on error
	//
	// Creates a depth stencil state base on the incoming information
	int CreateDepthStencilState(std::wstring name, D3D11_DEPTH_STENCIL_DESC* desc);

	// IN: D3D11_VIEWPORT - viewport to add
	// OUT: UINT - ID of the added viewport
	//
	// Adds the incoming viewport to the list of viewports
	UINT AddViewport(D3D11_VIEWPORT viewport);

	//---------------------------------------------------------------------------

	// IN: wstring - name of requested shader
	// OUT: DX11Shader* - pointer to the requested shader, nullptr on error
	//
	// Returns the shader of corresponding name
	DX11Shader* GetShader(std::wstring name);
	// IN: ShaderType - type of shader
	//     UINT       - ID of requested shader
	// OUT: DX11Shader* - pointer to requested shader, nullptr on error
	//
	// Returns the shader at the requested ID
	DX11Shader* GetShaderByID(ShaderType type, UINT ID);
	// IN: ShaderType - type of shader
	//     string     - name of requested shader
	// OUT: int - ID of the requested shader, -1 for error
	//
	// Returns the shader with the corresponding name
	int GetShaderByName(ShaderType type, std::string name);
	// IN: ShaderType - type of shader
	//     wstring    - name of requested shader
	// OUT: int - ID of the shader, -1 for error
	//
	// Returns the shader with the corresponding name
	int GetShaderByName(ShaderType type, std::wstring name);

	// IN: UINT - ID to use
	// OUT: DX11ContantBuffer* - Pointer to the requested buffer, nullptr if error
	//
	// Returns the constant buffer corresponding to the corresponding ID
	DX11ConstantBuffer* GetConstantBufferByID(UINT ID);
	// IN: string - name of buffer
	// OUT: int - ID of the requested buffer, nullptr if error
	//
	// Returns the id to the constant buffer corresponding to the incoming name
	int GetConstantBufferByName(std::string name);
	// IN: wstring - name of buffer
	// OUT: int - ID of the requested buffer, nullptr if error
	//
	// Returns the id to the constant buffer corresponding to the incoming name
	int GetConstantBufferByName(std::wstring name);

	// IN: UINT - ID of buffer
	// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
	//
	// Returns the requested buffer for the passed in type and ID
	DX11Buffer* GetBufferByID(UINT ID);
	// IN: BufferType - Type to look for
	//     UINT       - ID of buffer
	// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
	//
	// Returns the requested buffer for the passed in type and ID
	DX11Buffer* GetBufferByID(BufferType type, UINT ID);
	// IN: BufferType - Type to look for
	//     string     - name of buffer
	// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
	//
	// Returns the requested buffer for the passed in name and type
	int GetBufferByName(BufferType type, std::string name);
	// IN: BufferType - Type to look for
	//     wstring    - name of buffer
	// OUT: DX11Buffer* - Pointer to requested buffer, nullptr on error
	//
	// Returns the requested buffer for the passed in name and type
	int GetBufferByName(BufferType type, std::wstring name);

	// IN: UINT - ID of Input Layout
	// OUT: DX11InputLayout* - pointer to Input Layout, nullptr on error
	//
	// Returns the requested Input Layout
	DX11InputLayout* GetInputLayouByID(UINT ID);
	// IN: string - name of the layout
	// OUT: int - id of the layout, -1 on error
	//
	// Returns the ID for the requested Input Layout
	int GetInputLayoutByName(std::string name);
	// IN: wstring - name of the layout
	// OUT: int - id of the layout, -1 on error
	//
	// Returns the ID for the requested Input Layout
	int GetInputLayoutByName(std::wstring name);

	// IN: UINT - ID of Image
	// OUT: DX11ShaderResourceView* - pointer to image, nullptr on error
	//
	// Returns the requested image
	DX11ShaderResourceView* GetImageByID(UINT ID);
	// IN: string - name of image
	// OUT: int - ID of image
	//
	// Returns the requested image ID
	int GetImageByName(std::string name);
	// IN: wstring - name of image
	// OUT: int - ID of image
	//
	// Returns the requested image ID
	int GetImageByName(std::wstring name);
	
	// IN: SHADER_RESOURCE_TYPE - type of resource to get
	//     UINT                 - ID of Shader Resource
	// OUT: DX11ShaderResourceView* - pointer to Shader Resource, nullptr on error
	//
	// Returns the requested shader resource
	DX11ShaderResourceView* GetShaderResourceByID(SHADER_RESOURCE_TYPE type, UINT ID);
	// IN: SHADER_RESOURCE_TYPE - type of resource to get
	//     string               - name of Shader Resource
	// OUT: int - ID of Shader Resource
	//
	// Returns the requested shader resource ID
	int GetShaderResourceByName(SHADER_RESOURCE_TYPE type, std::string name);
	// IN: SHADER_RESOURCE_TYPE - type of resource to get
	//     wstring              - name of Shader Resource
	// OUT: int - ID of Shader Resource
	//
	// Returns the requested shader resource ID
	int GetShaderResourceByName(SHADER_RESOURCE_TYPE type, std::wstring name);

	// IN: UINT - ID of Unordered Access View
	// OUT: DX11UnorderedAccessView* - pointer to Unordered Access View, nullptr on error
	//
	// Returns the requested Unordered Access View
	DX11UnorderedAccessView* GetUnorderedAccessViewByID(UINT ID);
	// IN: string - name of Unordered Access View
	// OUT: int - ID of Unordered Access View
	//
	// Returns the requested Unordered Access View ID
	int GetUnorderedAccessViewByName(std::string name);
	// IN: wstring - name of Unordered Access View
	// OUT: int - ID of Unordered Access View
	//
	// Returns the requested Unordered Access View ID
	int GetUnorderedAccessViewByName(std::wstring name);

	// IN: UINT - ID of Render Target View
	// OUT: ID3D11RenderTargetView* - pointer to Render Target View, nullptr on error
	//
	// Returns the requested Render Target View
	DX11RenderTargetView* GetRenderTargetViewByID(UINT ID);
	// IN: string - name of the Render Target View
	// OUT: int - id of the Render Target View, -1 on error
	//
	// Returns the ID for the requested Render Target View
	int GetRenderTargetViewByName(std::string name);
	// IN: wstring - name of the Render Target View
	// OUT: int - id of the Render Target View, -1 on error
	//
	// Returns the ID for the requested Render Target View
	int GetRenderTargetViewByName(std::wstring name);

	// IN: UINT - ID of Depth Stencil View
	// OUT: ID3D11DepthStencilView* - pointer to Depth Stencil View, nullptr on error
	//
	// Returns the requested Depth Stencil View
	DX11DepthStencilView* GetDepthStencilViewByID(UINT ID);
	// IN: string - name of the Depth Stencil View
	// OUT: int - id of the Depth Stencil View, -1 on error
	//
	// Returns the ID for the requested Depth Stencil View
	int GetDepthStencilViewByName(std::string name);
	// IN: wstring - name of the Depth Stencil View
	// OUT: int - id of the Depth Stencil View, -1 on error
	//
	// Returns the ID for the requested Depth Stencil View
	int GetDepthStencilViewByName(std::wstring name);

	// IN: UINT - ID of Sampler State
	// OUT: ID3D11SamplerState* - pointer to Sampler State, nullptr on error
	//
	// Returns the requested Sampler State
	DX11SampleState* GetSamplerStateByID(UINT ID);
	// IN: string - name of the SamplerState
	// OUT: int - id of the SamplerState, -1 on error
	//
	// Returns the ID for the requested SamplerState
	int GetSamplerStateByName(std::string name);
	// IN: wstring - name of the SamplerState
	// OUT: int - id of the SamplerState, -1 on error
	//
	// Returns the ID for the requested SamplerState
	int GetSamplerStateByName(std::wstring name);

	// IN: UINT - ID of Rasterizer State
	// OUT: DX11RasterizerState* - pointer to Rasterizer State, nullptr on error
	//
	// Returns the requested Rasterizer State
	DX11RasterizerState* GetRasterizerStateByID(UINT ID);
	// IN: string - name of the Rasterizer State
	// OUT: int - id of the Rasterizer State, -1 on error
	//
	// Returns the ID for the requested Rasterizer State
	int GetRasterizerStateByName(std::string name);
	// IN: string - name of the Rasterizer State
	// OUT: int - id of the Rasterizer State, -1 on error
	//
	// Returns the ID for the requested Rasterizer State
	int GetRasterizerStateByName(std::wstring name);

	// IN: UINT - ID of Blend State
	// OUT: DX11BlendState* - pointer to Blend State, nullptr on error
	//
	// Returns the requested Blend State
	DX11BlendState* GetBlendStateByID(UINT ID);
	// IN: string - name of the Blend State
	// OUT: int - id of the Blend State, -1 on error
	//
	// Returns the ID for the requested Blend State
	int GetBlendStateByName(std::string name);
	// IN: wstring - name of the Blend State
	// OUT: int - id of the Blend State, -1 on error
	//
	// Returns the ID for the requested Blend State
	int GetBlendStateByName(std::wstring name);

	// IN: UINT - ID of Depth Stencil State
	// OUT: DX11DepthStencilState* - pointer to Depth Stencil State, nullptr on error
	//
	// Returns the requested Depth Stencil State
	DX11DepthStencilState* GetDepthStencilStateByID(UINT ID);
	// IN: string - name of the Depth Stencil State
	// OUT: int - id of the Depth Stencil State, -1 on error
	//
	// Returns the ID for the requested Depth Stencil State
	int GetDepthStencilStateByName(std::string name);
	// IN: wstring - name of the Depth Stencil State
	// OUT: int - id of the Depth Stencil State, -1 on error
	//
	// Returns the ID for the requested Depth Stencil State
	int GetDepthStencilStateByName(std::wstring name);

	// IN: UINT  - ID of buffer to update
	//     void* - pointer to the data to update
	//     UINT  - size of data in bytes
	// OUT: void
	//
	// Updates the sub resource requested if the resource is a valid buffer
	void UpdateSubResource(UINT ID, void* data, UINT size);

	// IN: int - ID of the buffer to copy to
	//     int - offset to use
	//     int - ID of the UVA that has a counter
	// OUT: void
	//
	// Copies the invisible counter on the UAV to the buffer
	void CopyStructureCount(int bufferID, int offset, int UAVID);

	// IN: UINT - ID of Viewport
	// OUT: D3D11_VIEWPORT* - pointer to Viewport, nullptr on error
	//
	// Returns the requested Viewport
	D3D11_VIEWPORT* GetViewportByID(UINT ID);
};

