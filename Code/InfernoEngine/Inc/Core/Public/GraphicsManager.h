// Author: Jonathan Gyurkovics
//
// Handles creating and upholding the game window and DirectX

#pragma once

#include "InfernoEngine.h"
#include "GlobalIncludes.h"
#include "Assets/Mesh.h"
#include "ShaderInfo.h"
#include "../../Utilities/Public/Color.h"
#include "Registry.h"
#include "Win32Window\Win32Window.h"
#include "Rendering\RenderTree.h"

#include <unordered_map>
#include <dxgitype.h>
#include <mutex>

template<typename T>
class Flags;
class Win32Window;

class FontManager;
class Font;
class SkinnedMeshRenderer;
class SpriteRenderer;
struct D2D_RECT_F;

class RenderingResources;
class PipelineManager;
class LightManager;
class ParticleManager;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;

#if defined(DEBUG) || defined(_DEBUG)
	struct ID3DUserDefinedAnnotation;
	struct IDXGIDebug;
#endif

class GraphicsManager
{
private:
	struct FreeVertexEntry
	{
		unsigned int startVertex;
		unsigned int totalVerts;
	};

	struct LoadedAssetData
	{
		char* Data;
		LoadedAssetData* Next;
	};

	enum GRAPHICS_FLAGS {
		ANTI_ALIASING_ENABLED,
		MSAA_ENBALED,
		VSYNC_ENABLED,
		TOTAL_FLAGS,
	};

private:
	std::vector<DXGI_MODE_DESC> m_vDisplayModes;

	MANAGER_STATUS m_eGraphicsStatus;			// Holds the status of the Graphics Manager

	Win32Window* m_pWindow;						// Holds a pointer to our window class
	float m_fWidth;								// Holds the width for DirectX
	float m_fHeight;							// Holds the height for DirectX
	float m_fAspectRatio;						// Holds the overal aspect ratio
	WINDOW_STYLE m_ePrevWindowStyle;			// Holds the previous window style
	float m_fPrevWidth;							// Holds the previous DirectX width
	float m_fPrevHeight;						// Holds the previous DirectX height
	UINT m_uiMSAACount;							// Holds the MSAA Count
	int m_iMSAAQuality;							// Holds the MSAA Quailty
	UINT m_uiAnisotropicCount;					// Holds the Anisotropic Count
	float m_fGamma;								// Holds the Gamma value
	LoadedAssetData* m_lHead;					// Holds the pointer to the head of loaded asset data
	std::mutex m_mDataMutex;					// Mutex for the linked list holding loaded asset data
	Flags<char>* m_fcGraphicsFlags;				// Holds all graphics related flags

#if defined(DEBUG) || defined(_DEBUG)
	ID3DUserDefinedAnnotation* m_pAnnotation;	// Pointer to the custom annotations for DirectX calls
	IDXGIDebug* m_pDebug;						// Pointer to the DXGI Debug
#endif

	LightManager* m_pLightManager;				// Holds the pointer to the light manager
	ParticleManager* m_pParticleManager;		// Holds the pointer to the Particle manager
	RenderTree* m_pRenderTree;					// Holds the point to the render tree for frustum culling

	// DirectX 11 variables
	ID3D11Device* m_pDevice;					// Holds the current DirectX 11 Device
	ID3D11DeviceContext* m_pContext;			// Holds the current DirectX 11 Device Context
	IDXGISwapChain* m_pSwapChain;				// Holds the current DirectX 11 Swap Chain
	IDXGIOutput* m_pVideoOutput;				// Holds the pointer to the monitor to use
	PipelineManager* m_pPipelineManager;		// Pointer to the pipeline manager
	RenderingResources* m_pResources;			// Pointer to the resource bank for DirectX

	// Temp-- Holds onto the UI InputLayout
	int UIInputID;
	int UIVertexID;
	int UIGeometryID;

	// Other temp variables
	int SkinnedVertexBufferID;

	// Depth Pass IDs
	// These IDs are needed so we can set up the pipeline to do our depth pre-pass
	int m_iRenderTarget;
	int m_iDepthStencilView;
	int m_iDepthPrepassDepthStencilState;
	int m_iPositionOnlyInputLayout;
	int m_iPositionOnlyVS;
	int m_iAlphaInputLayout;
	int m_iAlphaTestVertexShader;
	int m_iAlphaTestPixelShader;
	int m_iAlphaDepthStencilState;

	// Commonly used Constant Buffer pointers so we don't have to search for them each time we want to apply changes -TEMP
	ID3D11Buffer* WorldBuffer;
	ID3D11Buffer* ViewBuffer;
	ID3D11Buffer* BillboardBuffer;
	ID3D11Buffer* ProjectionBuffer;
	ID3D11Buffer* BonesBuffer;
	ID3D11Buffer* ColorBuffer;
	ID3D11Buffer* UVOffsetBuffer;

	// Vertex and Index Buffer pointers -TEMP
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* SkinnedVertexBuffer;
	ID3D11Buffer* IndexBuffer;

	std::unordered_map<Vertex, size_t> m_umUniqueVertsMap;				// Holds all unique verts and the index they sit at
	size_t m_uiPrevVertCount;											// Holds the count of verts since the last time they were added
	size_t m_uiPrevIndexCount;											// Holds the count of indices since the last time they were added

	std::vector<FreeVertexEntry> m_vVertexEntryList;					// Holds a list of free vertices for writing skinning information to

	Registry<Renderer> m_rRendererRegistery;					// Registry for all renderable objects
	Registry<Renderer> m_rAlphaRendererRegistery;				// Registry for all the alpha blending renderers
	Registry<SkinnedMeshRenderer> m_rSkinnedRendererRegistry;	// Registry for all the skinned mesh renderers
	Registry<UIRenderer> m_rUIRegistry;							// Registry for all UI renerables

	// Temp font information
	std::vector<std::tuple<Font*, std::wstring, D2D_RECT_F>> m_vRenderStrings;

	// IN: void
	// OUT: bool - States if we were able to create our device or not
	//
	// Creates our device and context
	bool InitializeDevice();

	// IN: void
	// OUT: bool - States window creation success
	//
	// Creates a window for the game to use
	//bool CreateGameWindow();

	// IN: void
	// OUT: bool - States DirectX setup success
	//
	// Sets up DirectX
	bool InitializeDirectX();

	// IN: IDXGIFactory1** - Pointer to the Factory Pointer
	//     IDXGIAdapter1** - Pointer to the Adapter Pointer
	// OUT: bool - Success on finding the adapter
	//
	// Finds the bets adapter to use
	bool FindAdapter(IDXGIFactory1** factory, IDXGIAdapter1** adapter);

	// IN: DXGI_MODE_DESC& - Output Mode to check against
	// OUT: void
	//
	// Gets all supported output modes and applies closest match
	void InitOutputModes(DXGI_MODE_DESC& mode);

	// IN: DXGI_MODE_DESC& - Output Mode to check against
	// OUT: void
	//
	// Updates the output mode to the closest mode that is supported
	void GetClosestOutputMode(DXGI_MODE_DESC& mode);

	// IN: Vertex*      - Array of Vertices to add
	//     unsigned int - Number of elements in the array
	// OUT: unsigned int - the start vertex in the vertex buffer
	//
	// Adds the verts to the vertex buffer and returns the location of the start vertex
	unsigned int LoadVerts(const Vertex* verts, const size_t numVerts);
	// IN: unsigned int* - Array of Indices to add
	//     unsigned int  - Number of elements in the array
	// OUT: unsigned int - Start location into the index list for the mesh
	//
	// Adds the indices to the index buffer and returns the start index
	unsigned int LoadIndices(unsigned int* indexList, size_t numIndicies);
	// IN: void* - Mesh to queue for loading
	//     bool  - If the mesh should be finalized, in some cases we don't want to as with NavMeshes
	// OUT: void
	//
	// Adds a mesh to the queue to be loaded. This should be used by seperate Threads.
	void QueueLoadMesh(void* mesh, bool finalize);
	// IN: wchar_t* - Name of the texture to load
	// OUT: ID3D11ShaderResourceView* - SRV Pointer for the image
	//
	// Loads the texture from the name that is passed in.
	// *NOTE* Do not use this with a seperate thread!!! Use StoreTextureInMemory instead
	int LoadTexture(const wchar_t* filename);
	// IN: wchar_t* - Name of hte texture to store in memory
	// OUT: void
	//
	// Loads the texture file into memory to be loaded onto the GPU at a later point. This automatically
	// queues the texture file for loading and should be used by Threads.
	void StoreTextureInMemory(const wchar_t* filename);

	// IN: void
	// OUT: void
	//
	// Loops through the linked list and loads data that is pending to load to the GPU. This is only called
	// by the GraphicsManager strictly.
	void LoadPendingAssets();

	// IN: Mesh* - pointer to the mesh requesting verts
	// OUT: vector<unsigned int> - List of offsets, size 0 on error
	//
	// Evaluates the mesh and available skinning verts and returns a list of offsets for writing
	// each submesh to the skinning vertex buffer
	std::vector<unsigned int> RequestSkinningVerts(Mesh* mesh);
	// IN: Mesh*        - pointer to the mesh releasing verts
	//     unsigned int - first offset of the lsit
	// OUT: void
	//
	// Releases the vertices the mesh is writing so other meshes may use them
	void ReleaseSkinningVerts(Mesh* mesh, unsigned int firstOffset);

	// IN: size_t - index of image
	//     size_t - slot for image
	// OUT: bool - status of the texture assign
	//
	// Assigns the texture at the index to the passed in slot
	bool SetTexture(ShaderType type, int index, int slot) const;

	// IN: void* - Pointer to the view matrix
	// OUT: void
	//
	// Registered function to receive the updated view matrix when it changes
	void UpdateViewMatrix(void* data);
	// IN: void* - Pointer to the projeciton matrix
	// OUT: void
	//
	// Registed function to receive the updated projection matrix when it changes
	void UpdateProjectionMatrix(void* data);

	// IN: bool - Setting of Fullscreen
	// OUT: void
	//
	// Sets the window to the desired setting
	void SetFullscreen(bool fullscreen);

	// IN: UINT - width of the target
	//     UINT - height of the target
	// OUT: void
	//
	// Resizes the target to the passed in dimensions
	void ResizeTarget(UINT width, UINT height);

	// IN: void
	// OUT: bool - Resize success
	//
	// Resize functions for the window
	bool ResizeWindow();
	// IN: width  - width in pixels of the screen
	//     height - height in pixels of the screen
	// OUT: bool - Resize success
	//
	// Resize functions for the window
	bool ResizeWindow(float width, float height);

	// IN: void
	// OUT: bool - success of destroying the window
	//
	// Destroys our current window
	bool DestroyWindow();

	// IN: Void
	// OUT: void
	//
	// Sorts all the blending renderers so blending is done properly
	void SortBlendingRenderers(std::vector<Renderer*>& renderers);

#if defined(DEBUG) || defined(_DEBUG)
	void StartEvent(const wchar_t* name);
	void EndEvent();
#endif

	// Remove the copy and assignement operators
	GraphicsManager(const GraphicsManager&) = delete;
	GraphicsManager& operator=(const GraphicsManager&) = delete;

public:
	// Hide our ctor and dtor
	GraphicsManager();
	~GraphicsManager();

	// IN: HINSTANCE - Instance of the application
	//     WNDPROC   - Windows message processing function pointer
	//     LPCWSTR   - Icon ID as string pointer  *OPTIONAL*
	//     LPCWSTR   - Small Icon ID as string pointer *OPTIONAL*
	//     LPCWSTR   - Menu name as string pointer *OPTIONAL*
	//     LPCWSTR   - Class name as string pointer *OPTIONAL*
	// OUT: bool - The success of initializing the manager
	//
	// Initializes the graphics manager
	bool Initialize(HINSTANCE hInstance, IWndProc* WndProc, LPCWSTR IconName = nullptr, LPCWSTR SmallIconName = nullptr, LPCWSTR className = nullptr);
	// IN: void
	// OUT: void
	//
	// Handles any post Initialize operations
	void PostInit();
	// IN: void
	// OUT: bool - Success of destroying to manager
	//
	// Destroys the current manager data
	bool Destroy();

	// IN: float - delta time
	// OUT: bool - Success of the update
	//
	// Updates the current instance
	bool Update();

	// IN: Font*       - Pointer to the font to use
	//     wstring     - string of text to render
	//     D2D1_RECT_F - Drawling rectangle region
	// OUT: void
	//
	// Adds the string and region to a list for the font type to render
	void RenderText(Font* font, std::wstring text, D2D_RECT_F* rectRegion);

	// IN: Renderer* - Renderer to register
	// OUT: void
	//
	// Registers the incoming renderer for rendering
	void RegisterRenderer(Renderer* renderer);
	// IN: UIRenderer* - Renderer to register
	// OUT: void
	//
	// Registers the incoming renderer for rendering
	void RegisterUIRenderer(UIRenderer* renderer);

	// IN: Light* - Pointer to the Light Component that needs to be registered
	// OUT: bool - Whether or not the lComponent was able to be registered
	// 
	// Registers the given Light with the Light Manager. Returns true if it was registered, false otherwise.
	bool RegisterLightComponent(Light* _lComponent);

	// IN: ParticleSystem* - Pointer to the Particle System Component that needs to be registered
	// OUT: bool - Whether or not the Component was able to be registered
	//
	// Registers the given Particle System with the Particle Manager.
	bool RegisterParticleSystem(ParticleSystem* _pComponent);

	// IN Renderer* - Renderer to unregister
	// OUT: void
	//
	// Unregisters a renderer
	void UnregisterRenderer(Renderer* renderer);
	// IN UIRenderer* - Renderer to unregister
	// OUT: void
	//
	// Unregisters a renderer
	void UnregisterUIRenderer(UIRenderer* renderer);

	//  IN : Light* - Pointer to the Light Component to unregistered
	//  OUT : bool - Whether or not it was unregistered
	// 
	//  Unregisters the given Light with the Light Manager. Returns true if it was registered, false otherwise.
	bool UnregisterLightComponent(Light* _lComponent);

	// IN: ParticleSystem* - Pointer to the Particle System Component to unregister
	// OUT: bool - Whether or not the Component was unregistered
	//
	// Uregisters the given Particle System with the Particle Manager.
	bool UnregisterParticleSystem(ParticleSystem* _pComponent);

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

	// IN: int                  - ID of the shader resource view
	//     UINT                 - Size of each element in bytes
	//     UINT                 - Flags to apply to the UAV
	// OUT: int - ID of the created UAV, -1 on error
	//
	// Creates an unordered access view for the requested shader resource view
	int CreateUnorderedAccessView(int textureID, UINT Flags, UINT format = 0);

	// IN: wstring - Name of the texture
	//     UINT    - Width of the texture
	//     UINT    - Height of the texture
	//     UINT    - Size of the border, 0 for none
	//     Color   - Color of the background
	//     Color   - Color of the boarder if border size is greater then 0
	// OUT: int - ID of the image that was created
	//
	// Creates a solid color image with a border if one is requested
	int CreateSolidColorImage(std::wstring name, UINT width, UINT height, UINT borderSize, Color backColor = Color(1, 1, 1, 1), Color borderColor = Color(0, 0, 0, 1));

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

	// IN: void
	// OUT: void
	//
	// Updates the window if any of the window settings changed
	void WindowUpdate();
	// IN: void
	// OUT: void
	//
	// Accept changes that have been made to the window
	void ApplyWindowChanges();
	// IN: void
	// OUT: void
	//
	// Revert changes made to the board if ApplyWindowChanges hasn't been called
	void RevertWindowChanges();

	// IN: void
	// OUT: void
	//
	// Applies the current pipeline state
	void ApplyPipelineState();

	// IN: Matrix4 - World Matrix to apply
	// OUT: void
	//
	// Applies the passed in world matrix to the world buffer
	void SetWorldMatrix(const Matrix4& world);
	// IN: Matrix4 - View Matrix to apply
	// OUT: void
	//
	// Applies the passed in view matrix to the view buffer
	void SetViewMatrix(const Matrix4& view);
	// IN: Matrix4 - Projection Matrix to apply
	// OUT: void
	//
	// Applies the passed in projection matrix to the projection buffer
	void SetProjectionMatrix(const Matrix4& projection);
	// IN: COLOR - Color to apply
	// OUT: void
	//
	// Applies the passed in color to the color buffer
	void SetColor(const Color& color);
	// IN: Vector2 - UV Offset to apply
	// OUT: void
	//
	// Applies the passd in uv offsets to the offset buffer
	void SetUVOffsets(const Vector2& offset, const Vector2& tiling);

	// IN: unsigned int - ID of the layout
	// OUT: void
	//
	// Assigns the requested input layout to the InputAssembler
	void SetInputLayout(unsigned int ID);
	// IN: ShaderType - type of shader to apply
	//     int        - ID to the shader being applied
	// OUT: void
	//
	// Sets the current shader type at the passed in ID to the pipeline
	void SetShader(ShaderType type, int ID);
	// IN: ShaderType    - type of shader to apply to
	//     unsigned int  - starting slot
	//     unsigned int  - total slots used
	//     ID3D11Buffer* - pointer to the starting buffer
	// OUT: void
	//
	// Applies all the passed in buffers to the pipeline
	void SetConstantBuffers(ShaderType type, unsigned int startSlot, unsigned int totalBuffers, ID3D11Buffer** startBuffer);
	// IN: ShaderType   - Type of Shader to apply to
	//     unsigned int - ID of the constant buffer
	//     unsigned int - Slot of the buffer
	// OUT: void
	//
	// Assigns the passed in constant buffer to the pipeline
	void SetConstantBuffer(ShaderType type, unsigned int ID, unsigned int Slot);
	// IN: unsigned int - ID of the buffer
	//     unsigned int - stride of the buffer in bytes
	//     unsigned int - offset of the buffer in bytes
	// OUT: void
	//
	// Applies the passed in vertex buffer to the InputAssembler

	void SetVertexBuffer(unsigned int ID, unsigned int stride, unsigned int offset);
	// IN: unsigned int - ID of the buffer
	// OUT: void
	//
	// Applies the passed in index buffer to the InputAssembler
	void SetIndexBuffer(unsigned int ID);
	// IN: ShaderType - Shader to apply UAV to
	//     int        - UAV ID
	//     int        - Slot to apply
	// OUT: void
	//
	// Applies a Unordered Access View to the requested slot of the requested shader type
	void SetUnorderedAccessView(ShaderType type, int ID, int Slot, int Count = -1);
	// IN: ShaderType - type of shader to apply to
	//     int        - ID of the sampler to apply
	//     int        - Slot for the sampler
	// OUT: void
	//
	// Applies all the passed in states to the pipeline
	void SetSamplerStates(ShaderType type, int ID, int Slot);
	// IN: ShaderType                - type of shader to apply to
	//     unsigned int              - starting slot
	//     unsigned int		         - total slots used
	//     ID3D11ShaderResourceView* - pointer to the starting view
	// OUT: void
	//
	// Applies all the passed in views to the pipeline
	void SetShaderResourceViews(ShaderType type, unsigned int startSlot, unsigned int totalViews, ID3D11ShaderResourceView** startView);
	// IN: TopologyType - Topology to set
	// OUT: void
	//
	// Applies the requested topology
	void SetTopology(TopologyType topology);
	// IN: int - ID of Blend State
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetBlendState(int ID);
	// IN: int - ID of the Rasterizer State
	// OUT: void
	//
	// Updates the desired state with the incoming information
	void SetRasterizerState(int ID);
	// IN: int  - ID of the Depth Stencil State
	//     UINT - Reference value for the stencil
	// OUT: void
	//
	// Assigns the requested depth stencil state and ref value to the pipeline
	void SetDepthStencilState(int ID, UINT ref);

	// IN: int - ID of the Depth Stencil View
	// OUT: void
	//
	// Assigns the requested depth stencil view to the pipeline
	void SetDepthStencilView(int ID);

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

	// Draw calls that don't use the pipeline manager
	// ------------------------------------------------------------------------------------------

	// IN: unsigned int - Number of Vertices to render
	//     unsigned int - Start Vertex
	// OUT: void
	//
	// Draws the vertices with the current pipeline setup
	void Draw(unsigned int numVertices, unsigned int startVertex);

	// IN: unsigned int - Number of Indices to render
	//     unsigned int - Start Index
	//     unsigned int - Start Vertex
	// OUT: void
	//
	// Draws the indices using the ccurrent pipeline setup
	void DrawIndexed(unsigned int numIndices, unsigned int startIndex, unsigned int startVertex);

	//--------------------------------------------------------------------------------------------

	// IN: int   - ID of the effect to use
	//     Mesh* - Pointer to the mesh
	// OUT: void
	//
	// Draws the mesh using the requested effect
	void Draw(int EffectID, Mesh* mesh);

	// IN: int - ID of the effect to use
	//     int - Start Vertex
	//     int - Num Vertices
	// OUT: void
	//
	// Calls a standard draw call using the passed in effect ID
	void Draw(int EffectID, int StartVertex, int NumVerts);

	// IN: int          - ID of the effect to use
	//     unsigned int - start index
	//     unsinged int - num indices
	//     unsigned int - start vertex
	// OUT: void
	//
	// Draws the passed in mesh using the effect requested
	void DrawIndexed(int EffectID, unsigned int startIndex, unsigned int numIndices, unsigned int startVertex);

	// IN: int  - ID of the buffer with the draw arguments
	//     UINT - offset in bytes for the draw arguments
	// OUT: void
	//
	// Applies changes to the pipeline and uses the buffer from the passed in ID to draw
	void DrawIndirect(int BufferID, UINT offset);

	// IN: unsigned int - X thread groups
	//     unsigned int - Y thread groups
	//     unsigned int - Z thread groups
	// OUT: void
	//
	// Calls dispatch on the current pipeline
	void Disptach(unsigned int x, unsigned int y, unsigned int z);

	void SetAmbientColor(float r, float g, float b);
	void SetAmbientColor(Vector3 color);
	void SetAmbientColor(Vector4 color);
	void SetAmbientIntensity(float intensity);

	// IN: char* - name of the effect
	// OUT: int - ID of the effect
	//
	// Returns the ID for the effect of the corresonding name if it exists
	int GetEffectByName(const char* name) const;

	// IN: ShaderType - type of shader to grab
	//     wstring    - name of shader
	// OUT: int - ID of shader, -1 on error
	//
	// Returns the ID of requested shader if it exists
	int GetShader(ShaderType type, std::wstring name);
	// IN: wstring - name of buffer
	// OUT: int - ID of buffer, -1 on error
	//
	// Returns the ID of requested buffer if it exists
	int GetConstantBuffer(std::wstring name);
	// IN: BufferType - type of buffer
	//     wstring    - name of buffer
	// OUT: int - ID of buffer, -1 on error
	//
	// Returns the ID of requested buffer if it exists
	int GetBuffer(BufferType type, std::wstring name);
	// IN: wstring - name of image
	// OUT: int - ID of image, -1 on error
	//
	// Returns the ID of requested image if it exists
	int GetImage(std::wstring name);
	// IN: SHADER_RESOURCE_TYPE - type of resource to get
	//     wstring              - name of Shader Resource
	// OUT: int - ID of Shader Resource
	//
	// Returns the requested shader resource ID
	int GetShaderResource(SHADER_RESOURCE_TYPE type, std::wstring name);
	// IN: wstring - name of Unordered Access View
	// OUT: int - ID of Unordered Access View
	//
	// Returns the requested Unordered Access View ID
	int GetUnorderedAccessView(std::wstring name);
	// IN: wstring - name of input layout
	// OUT: int - ID of input layout, -1 on error
	//
	// Returns the ID of requested input layout if it exists
	int GetInputLayout(std::wstring name);
	// IN: wstring - name of render target view
	// OUT: int - ID of render target view, -1 on error
	//
	// Returns the ID of requested render target view if it exists
	int GetRenderTargetView(std::wstring name);
	// IN: wstring - name of depth stencil view
	// OUT: int - ID of depth stencil view, -1 on error
	//
	// Returns the ID of requested depth stencil view if it exists
	int GetDepthStencilView(std::wstring name);
	// IN: wstring - name of sampler state
	// OUT: int - ID of sampler state, -1 on error
	//
	// Returns the ID of requested sampler state if it exists
	int GetSamplerState(std::wstring name);
	// IN: wstring - name of rasterizer state
	// OUT: int - ID of rasterizer state, -1 on error
	//
	// Returns the ID of requested rasterizer state if it exists
	int GetRasterizerState(std::wstring name);
	// IN: wstring - name of blend state
	// OUT: int - ID of the blend state, -1 on error
	//
	// Returns the ID of the requested blend state if it exists
	int GetBlendState(std::wstring name);
	// IN: wstring - name of the depth stencil state
	// OUT: int - ID of the depth stencil state, -1 on error
	//
	// Returns the ID of the requested depth stencil state if it exists
	int GetDepthStencilState(std::wstring name);

	// IN: void
	// OUT: void
	//
	// Destroys all active particles
	void CleanUpParticles();

	// Accessors and Mutators for Windows style stuff

	void SetWindowInfo(WINDOW_STYLE style, float width, float height);
	void SetWindowStyle(WINDOW_STYLE style);
	void SetWindowDimensions(float width, float height);

	WINDOW_STYLE GetWindowStyle() const;
	float GetWindowWidth() const;
	float GetWindowHeight() const;
	float GetAspectRatio() const;
	UINT GetNumDisplays() const;
	void GetSupportedResolutions(std::vector<std::wstring>& resolutions, UINT displayID = 0) const;

	// Other Accessors and Mutators

	void SetVSync(bool vsync);
	void SetMSAACount(unsigned int count);
	void SetMSAAQuality(int quality);
	void SetLineAA(bool lineAA);
	void SetAnisotropicFiltering(unsigned int anisotropic);
	void SetGamma(float gamma);

	HWND Window() const;
	HINSTANCE Instance() const;
	bool IsWindowDirty() const;
	bool VSyncEnabled() const;
	unsigned int GetMSAACount() const;
	int GetMSAAQuality() const;
	bool AntiAliasingEnabled() const;
	unsigned int GetAnisotropicFiltering() const;
	float GetGamma() const;
	bool MSAAEnabled() const;

	friend class InfernoEngine;
};

