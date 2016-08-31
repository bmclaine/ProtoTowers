#include "../Public/ModuleInterface.h"

#include "InfernoEngine.h"
#include "InputManager.h"
#include "MathLib.h"
#include "Color.h"
#include "PrefabLoader.h"
#include "../Public/Physics/Collision.h"

InfernoEngine* g_pEngineInstance = nullptr;

namespace InfernoEngine_AI
{
	// === RegisterNavAgent
	//  IN : NavAgent* - Pointer to the agent that needs to be registered
	//  OUT : bool - Whether or not the agent was able to register 
	//
	//  Registers the given NavAgent with the AI Module. Returns true if it was registered, false otherwise.
	bool RegisterNavAgent(NavAgent* _navAgent)
	{
		return g_pEngineInstance->RegisterNavAgent(_navAgent);
	}

	// === RegisterAIEntity
	// IN : AIEntity* - Pointer to the obstacle that needs to be registered
	// OUT : bool - Whether or not the obstacle was able to be registered
	//
	// Registers the given AIEntity with the AI Module. Returns true if it was registered, false otherwise
	bool RegisterAIEntity(AIEntity* _entity)
	{
		return g_pEngineInstance->RegisterAIEntity(_entity);
	}

	// === UnregisterNavAgent
	//  IN : NavAgent* - Pointer to the NavAgent to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given NavAgent from the AI Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterNavAgent(NavAgent* _navAgent)
	{
		return g_pEngineInstance->UnregisterNavAgent(_navAgent);
	}

	// === UnregisterAIEntity
	//  IN : AIEntity* - Pointer to the AIEntity to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  UNregisters the given AIEntity from the AI Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterAIEntity(AIEntity* _entity)
	{
		return g_pEngineInstance->UnregisterAIEntity(_entity);
	}

	// === AIModule::EnterCutscene
	//  IN : void
	//  OUT : void
	//
	// Tells the AIModule if we've entered a cutscene, to handle entities accordingly
	void EnterCutScene()
	{
		g_pEngineInstance->EnterCutscene();
	}

	// === AIModule::ExitCutscene
	//  IN : void
	//  OUT : void
	//
	// Tells the AIModule if we've entered a cutscene, to handle entities accordingly
	void ExitCutScene()
	{
		g_pEngineInstance->ExitCutscene();
	}
}

namespace InfernoEngine_Audio
{
	void SetMasterVolume(float volume)
	{
		g_pEngineInstance->SetMasterVolume(volume);
	}
	void SetSFXVolume(float volume)
	{
		g_pEngineInstance->SetSFXVolume(volume);
	}
	void SetBackgroundMusicVolume(float volume)
	{
		g_pEngineInstance->SetBackgroundMusicVolume(volume);
	}

	float GetMasterVolume()
	{
		return g_pEngineInstance->GetMasterVolume();
	}
	float GetSFXVolume()
	{
		return g_pEngineInstance->GetSFXVolume();
	}
	float GetBackgroundMusicVolume()
	{
		return g_pEngineInstance->GetBackgroundVolume();
	}
}

namespace InfernoEngine_Graphics
{
	// IN: void
	// OUT: void
	//
	// Updates the window if any of the window settings changed
	void WindowUpdate()
	{
		g_pEngineInstance->WindowUpdate();
	}
	// IN: void
	// OUT: void
	//
	// Accept changes that have been made to the window
	void ApplyWindowChanges()
	{
		g_pEngineInstance->ApplyWindowChanges();
	}
	// IN: void
	// OUT: void
	//
	// Revert changes made to the board if ApplyWindowChanges hasn't been called
	void RevertWindowChanges()
	{
		g_pEngineInstance->RevertWindowChanges();
	}

	// === RegisterRenderComponent
	//  IN : Renderer* - Pointer to the render component that needs to be registered
	//  OUT : bool - Whether or not the rComponent was able to be registered.
	// 
	//  Registers the given Renderer with the Graphics Manager. Returns true if it was registered, false otherwise.
	bool RegisterRenderComponent(Renderer* _rComponent)
	{
		return g_pEngineInstance->RegisterRenderComponent(_rComponent);
	}

	// === RegisterUIRenderComponent
	//  IN : UIRenderer* - Pointer to the UIRender Component that needs to be registered
	//  OUT : bool - Whether or not the rComponent was able to be registered
	//
	//  Registers the given UIRenderer with the Graphics Manager. Returns true if it was registered, false otherwise.
	bool RegisterUIRenderComponent(UIRenderer* _rComponent)
	{
		return g_pEngineInstance->RegisterUIRenderComponent(_rComponent);
	}

	// === RegisterLightComponent
	//  IN : Light* - Pointer to the Light Component that needs to be registered
	//  OUT : bool - Whether or not the lComponent was able to be registered
	// 
	//  Registers the given Light with the Light Manager. Returns true if it was registered, false otherwise.
	bool RegisterLightComponent(Light* _lComponent)
	{
		return g_pEngineInstance->RegisterLightComponent(_lComponent);
	}

	// IN: ParticleSystem* - Pointer to the Particle System Component that needs to be registered
	// OUT: bool - Whether or not the Component was able to be registered
	//
	// Registers the given Particle System with the Particle Manager.
	bool RegisterParticleSystem(ParticleSystem* _pComponent)
	{
		return g_pEngineInstance->RegisterParticleSystem(_pComponent);
	}

	// === UnregisterRenderComponent
	//  IN : Renderer* - The pointer to the Renderer Component to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given Renderer from the Graphics Manager. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterRenderComponent(Renderer* _rComponent)
	{
		return g_pEngineInstance->UnregisterRenderComponent(_rComponent);
	}

	// === UnregisterUIRenderComponent
	//  IN : UIRenderer* - The pointer to the UIRenderer Component to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given UIRenderer from the Graphics Manager. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterUIRenderComponent(UIRenderer* _rComponent)
	{
		return g_pEngineInstance->UnregisterUIRenderComponent(_rComponent);
	}

	// === UnregisterLightComponent
	//  IN : Light* - Pointer to the Light Component to unregistered
	//  OUT : bool - Whether or not it was unregistered
	// 
	//  Unregisters the given Light with the Light Manager. Returns true if it was registered, false otherwise.
	bool UnregisterLightComponent(Light* _lComponent)
	{
		return g_pEngineInstance->UnregisterLightComponent(_lComponent);
	}

	// IN: ParticleSystem* - Pointer to the Particle System Component to unregister
	// OUT: bool - Whether or not the Component was unregistered
	//
	// Uregisters the given Particle System with the Particle Manager.
	bool UnregisterParticleSystem(ParticleSystem* _pComponent)
	{
		return g_pEngineInstance->UnregisterParticleSystem(_pComponent);
	}

	// IN: void
	// OUT: void
	//
	// Applies the current pipeline state
	void ApplyPipelineState()
	{
		return g_pEngineInstance->ApplyPipelineState();
	}

	// IN: ShaderType - type of shader to grab
	//     wstring    - name of shader
	// OUT: int - ID of shader, -1 on error
	//
	// Returns the ID of requested shader if it exists
	int GetShader(ShaderType type, std::wstring name)
	{
		return g_pEngineInstance->GetShader(type, name);
	}
	// IN: wstring - name of buffer
	// OUT: int - ID of buffer, -1 on error
	//
	// Returns the ID of requested buffer if it exists
	int GetConstantBuffer(std::wstring name)
	{
		return g_pEngineInstance->GetConstantBuffer(name);
	}
	// IN: BufferType - type of buffer
	//     wstring    - name of buffer
	// OUT: int - ID of buffer, -1 on error
	//
	// Returns the ID of requested buffer if it exists
	int GetBuffer(BufferType type, std::wstring name)
	{
		return g_pEngineInstance->GetBuffer(type, name);
	}
	// IN: wstring - name of image
	// OUT: int - ID of image, -1 on error
	//
	// Returns the ID of requested image if it exists
	int GetImage(std::wstring name)
	{
		return g_pEngineInstance->GetImage(name);
	}
	// IN: SHADER_RESOURCE_TYPE - type of resource to get
	//     wstring              - name of Shader Resource
	// OUT: int - ID of Shader Resource
	//
	// Returns the requested shader resource ID
	int GetShaderResource(SHADER_RESOURCE_TYPE type, std::wstring name)
	{
		return g_pEngineInstance->GetShaderResource(type, name);
	}
	// IN: wstring - name of Unordered Access View
	// OUT: int - ID of Unordered Access View
	//
	// Returns the requested Unordered Access View ID
	int GetUnorderedAccessView(std::wstring name)
	{
		return g_pEngineInstance->GetUnorderedAccessView(name);
	}
	// IN: wstring - name of input layout
	// OUT: int - ID of input layout, -1 on error
	//
	// Returns the ID of requested input layout if it exists
	int GetInputLayout(std::wstring name)
	{
		return g_pEngineInstance->GetInputLayout(name);
	}
	// IN: wstring - name of render target view
	// OUT: int - ID of render target view, -1 on error
	//
	// Returns the ID of requested render target view if it exists
	int GetRenderTargetView(std::wstring name)
	{
		return g_pEngineInstance->GetRenderTargetView(name);
	}
	// IN: wstring - name of depth stencil view
	// OUT: int - ID of depth stencil view, -1 on error
	//
	// Returns the ID of requested depth stencil view if it exists
	int GetDepthStencilView(std::wstring name)
	{
		return g_pEngineInstance->GetDepthStencilView(name);
	}
	// IN: wstring - name of sampler state
	// OUT: int - ID of sampler state, -1 on error
	//
	// Returns the ID of requested sampler state if it exists
	int GetSamplerState(std::wstring name)
	{
		return g_pEngineInstance->GetSamplerState(name);
	}
	// IN: wstring - name of rasterizer state
	// OUT: int - ID of rasterizer state, -1 on error
	//
	// Returns the ID of requested rasterizer state if it exists
	int GetRasterizerState(std::wstring name)
	{
		return g_pEngineInstance->GetRasterizerState(name);
	}
	// IN: wstring - name of blend state
	// OUT: int - ID of the blend state, -1 on error
	//
	// Returns the ID of the requested blend state if it exists
	int GetBlendState(std::wstring name)
	{
		return g_pEngineInstance->GetBlendState(name);
	}
	// IN: wstring - name of the depth stencil state
	// OUT: int - ID of the depth stencil state, -1 on error
	//
	// Returns the ID of the requested depth stencil state if it exists
	int GetDepthStencilState(std::wstring name)
	{
		return g_pEngineInstance->GetDepthStencilState(name);
	}

	// IN: unsigned int - ID of the layout
	// OUT: void
	//
	// Assigns the requested input layout to the InputAssembler
	void SetInputLayout(unsigned int ID)
	{
		g_pEngineInstance->SetInputLayout(ID);
	}
	// IN: ShaderType - type of shader to apply
	//     int        - ID to the shader being applied
	// OUT: void
	//
	// Sets the current shader type at the passed in ID to the pipeline
	void SetShader(ShaderType type, int ID)
	{
		g_pEngineInstance->SetShader(type, ID);
	}
	// IN: ShaderType   - Type of Shader to apply to
	//     unsigned int - ID of the constant buffer
	//     unsigned int - Slot of the buffer
	// OUT: void
	//
	// Assigns the passed in constant buffer to the pipeline
	void SetConstantBuffer(ShaderType type, unsigned int ID, unsigned int Slot)
	{
		g_pEngineInstance->SetConstantBuffer(type, ID, Slot);
	}
	// IN: unsigned int - ID of the buffer
	//     unsigned int - stride of the buffer in bytes
	//     unsigned int - offset of the buffer in bytes
	// OUT: void
	//
	// Applies the passed in vertex buffer to the InputAssembler
	void SetVertexBuffer(unsigned int ID, unsigned int stride, unsigned int offset)
	{
		g_pEngineInstance->SetVertexBuffer(ID, stride, offset);
	}

	// IN: unsigned int - ID of the buffer
	// OUT: void
	//
	// Applies the passed in index buffer to the InputAssembler
	void SetIndexBuffer(unsigned int ID)
	{
		g_pEngineInstance->SetIndexBuffer(ID);
	}

	// IN: Matrix4 - World Matrix to apply
	// OUT: void
	//
	// Applies the passed in world matrix to the world buffer
	void SetWorldMatrix(const Matrix4& world)
	{
		g_pEngineInstance->SetWorldMatrix(world);
	}
	// IN: Matrix4 - View Matrix to apply
	// OUT: void
	//
	// Applies the passed in view matrix to the view buffer
	void SetViewMatrix(const Matrix4& view)
	{
		g_pEngineInstance->SetViewMatrix(view);
	}
	// IN: Matrix4 - Projection Matrix to apply
	// OUT: void
	//
	// Applies the passed in projection matrix to the projection buffer
	void SetProjectionMatrix(const Matrix4& projection)
	{
		g_pEngineInstance->SetProjectionMatrix(projection);
	}
	// IN: COLOR - Color to apply
	// OUT: void
	//
	// Applies the passed in color to the color buffer
	void SetColor(const Color& color)
	{
		g_pEngineInstance->SetColor(color);
	}
	// IN: Vector2 - UV Offset to apply
	// OUT: void
	//
	// Applies the passd in uv offsets to the offset buffer
	void SetUVOffsets(const Vector2& offset, const Vector2& tiling)
	{
		g_pEngineInstance->SetUVOffsets(offset, tiling);
	}
	// IN: ShaderType - Shader to apply image to
	//     int        - Image ID
	//     int        - Slot to apply
	// OUT: void
	//
	// Applies the requested image to the requested shader slot
	void SetImage(ShaderType type, int ID, int Slot)
	{
		g_pEngineInstance->SetImage(type, ID, Slot);
	}
	// IN: ShaderType - Shader to apply UAV to
	//     int        - UAV ID
	//     int        - Slot to apply
	// OUT: void
	//
	// Applies a Unordered Access View to the requested slot of the requested shader type
	void SetUnorderedAccessView(ShaderType type, int ID, int Slot, int Count)
	{
		g_pEngineInstance->SetUnorderedAccessView(type, ID, Slot, Count);
	}
	// IN: ShaderType - type of shader to apply to
	//     int        - ID of the sampler to apply
	//     int        - Slot for the sampler
	// OUT: void
	//
	// Applies all the passed in states to the pipeline
	void SetSamplerStates(ShaderType type, int ID, int Slot)
	{
		g_pEngineInstance->SetSamplerStates(type, ID, Slot);
	}

	// IN: TopologyType - Topology to set
	// OUT: void
	//
	// Applies the requested topology
	void SetTopology(TopologyType topology)
	{
		g_pEngineInstance->SetTopology(topology);
	}

	// IN: int - ID of Blend State
	// OUT: void
	//
	// Updates the desired state with incoming information
	void SetBlendState(int ID)
	{
		g_pEngineInstance->SetBlendState(ID);
	}

	// IN: int - ID of the Rasterizer State
	// OUT: void
	//
	// Updates the desired state with the incoming information
	void SetRasterizerState(int ID)
	{
		g_pEngineInstance->SetRasterizerState(ID);
	}

	// IN: int  - ID of the Depth Stencil State
	//     UINT - Reference value for the stencil
	// OUT: void
	//
	// Assigns the requested depth stencil state and ref value to the pipeline
	void SetDepthStencilState(int ID, UINT ref)
	{
		g_pEngineInstance->SetDepthStencilState(ID, ref);
	}

	// IN: int - ID of the Depth Stencil View
	// OUT: void
	//
	// Assigns the requested depth stencil view to the pipeline
	void SetDepthStencilView(int ID)
	{
		g_pEngineInstance->SetDepthStencilView(ID);
	}

	// IN: UINT  - ID of buffer to update
	//     void* - pointer to the data to update
	//     UINT  - size of data in bytes
	// OUT: void
	//
	// Updates the sub resource requested if the resource is a valid buffer
	void UpdateSubResource(unsigned int ID, void* data, unsigned int size)
	{
		g_pEngineInstance->UpdateSubResource(ID, data, size);
	}

	// IN: int - ID of the buffer to copy to
	//     int - offset to use
	//     int - ID of the UVA that has a counter
	// OUT: void
	//
	// Copies the invisible counter on the UAV to the buffer
	void CopyStructureCount(int bufferID, int offset, int UAVID)
	{
		g_pEngineInstance->CopyStructureCount(bufferID, offset, UAVID);
	}

	// Draw calls that don't use the pipeline manager
	// ------------------------------------------------------------------------------------------

	// IN: unsigned int - Number of Vertices to render
	//     unsigned int - Start Vertex
	// OUT: void
	//
	// Draws the vertices with the current pipeline setup
	void Draw(unsigned int numVertices, unsigned int startVertex)
	{
		g_pEngineInstance->Draw(numVertices, startVertex);
	}

	// IN: unsigned int - Number of Indices to render
	//     unsigned int - Start Index
	//     unsigned int - Start Vertex
	// OUT: void
	//
	// Draws the indices using the ccurrent pipeline setup
	void DrawIndexed(unsigned int numIndices, unsigned int startIndex, unsigned int startVertex)
	{
		g_pEngineInstance->DrawIndexed(numIndices, startIndex, startVertex);
	}

	//--------------------------------------------------------------------------------------------

	// IN: int   - ID of the effect to use
	//     Mesh* - Pointer to the mesh
	// OUT: void
	//
	// Draws the mesh using the requested effect
	void Draw(int EffectID, Mesh* mesh)
	{
		g_pEngineInstance->Draw(EffectID, mesh);
	}
	// IN: int - ID of the effect to use
	//     int - Start Vertex
	//     int - Num Vertices
	// OUT: void
	//
	// Calls a standard draw call using the passed in effect ID
	void Draw(int EffectID, int StartVertex, int NumVerts)
	{
		g_pEngineInstance->Draw(EffectID, StartVertex, NumVerts);
	}
	// IN: int          - ID of the effect to use
	//     unsigned int - start index
	//     unsinged int - num indices
	//     unsigned int - start vertex
	// OUT: void
	//
	// Draws the passed in mesh using the effect requested
	void DrawIndexed(int EffectID, unsigned int startIndex, unsigned int numIndices, unsigned int startVertex)
	{
		g_pEngineInstance->DrawIndexed(EffectID, startIndex, numIndices, startVertex);
	}
	// IN: int  - ID of the buffer with the draw arguments
	//     UINT - offset in bytes for the draw arguments
	// OUT: void
	//
	// Applies changes to the pipeline and uses the buffer from the passed in ID to draw
	void DrawIndirect(int BufferID, unsigned int offset)
	{
		g_pEngineInstance->DrawIndirect(BufferID, offset);
	}
	// IN: unsigned int - X thread groups
	//     unsigned int - Y thread groups
	//     unsigned int - Z thread groups
	// OUT: void
	//
	// Calls dispatch on the current pipeline
	void Disptach(unsigned int x, unsigned int y, unsigned int z)
	{
		g_pEngineInstance->Disptach(x, y, z);
	}

	// IN: Mesh* - Pointer to the mesh to load
	// OUT: void
	//
	// Loads the mesh data into the graphics card
	void LoadMesh(Mesh* mesh)
	{
		g_pEngineInstance->LoadMesh(mesh);
	}
	// IN: void* - Mesh to queue for loading
	//     bool  - If the mesh should be finalized, in some cases we don't want to as with NavMeshes
	// OUT: void
	//
	// Adds a mesh to the queue to be loaded. This should be used by seperate Threads.
	void QueueLoadMesh(void* mesh, bool finalize)
	{
		g_pEngineInstance->QueueLoadMesh(mesh, finalize);
	}

	// IN: wchar_t* - name of the texture to load
	// OUT: int - index into the texture array
	//
	// Loads a texture if it hasn't been loaded yet and returns the index into the textures array
	int LoadTexture(const wchar_t* filename)
	{
		return g_pEngineInstance->LoadTexture(filename);
	}
	// IN: wchar_t* - Name of hte texture to store in memory
	// OUT: void
	//
	// Loads the texture file into memory to be loaded onto the GPU at a later point. This automatically
	// queues the texture file for loading and should be used by Threads.
	void StoreTextureInMemory(const wchar_t* filename)
	{
		return g_pEngineInstance->StoreTextureInMemory(filename);
	}

	// IN: void* - pointer to a ShaderResourceView
	// OUT: int - ID of the Image, -1 on error
	//
	// Adds the passed in ShaderResourceView to the image list
	int AddImage(std::wstring name, void* image)
	{
		return g_pEngineInstance->AddImage(name, image);
	}

	// IN: wstring  - Name for the ShaderResource
	//     wstring* - Pointer to the array of image names
	//     uint     - number of image names
	// OUT: int - ID to the ShaderResourceView
	int CreateTextureArray(std::wstring name, std::wstring* textureNames, unsigned int numTextures)
	{
		return g_pEngineInstance->CreateTextureArray(name, textureNames, numTextures);
	}

	// IN: int                  - ID of the shader resource view
	//     UINT                 - Size of each element in bytes
	//     UINT                 - Flags to apply to the UAV
	// OUT: int - ID of the created UAV, -1 on error
	//
	// Creates an unordered access view for the requested shader resource view
	int CreateUnorderedAccessView(int textureID, UINT Flags, UINT format)
	{
		return g_pEngineInstance->CreateUnorderedAccessView(textureID, Flags, format);
	}

	// IN: wstring - Name of the texture
	//     UINT    - Width of the texture
	//     UINT    - Height of the texture
	//     UINT    - Size of the border, 0 for none
	//     Color   - Color of the background
	//     Color   - Color of the boarder if border size is greater then 0
	// OUT: int - ID of the image that was created
	//
	// Creates a solid color image with a border if one is requested
	int CreateSolidColorImage(std::wstring name, unsigned int width, unsigned int height, unsigned int borderSize, Color backColor, Color borderColor)
	{
		return g_pEngineInstance->CreateSolidColorImage(name, width, height, borderSize, backColor, borderColor);
	}

	// IN: Mesh* - pointer to the mesh requesting verts
	// OUT: vector<unsigned int> - List of offsets, size 0 on error
	//
	// Evaluates the mesh and available skinning verts and returns a list of offsets for writing
	// each submesh to the skinning vertex buffer
	std::vector<unsigned int> RequestSkinningVerts(Mesh* mesh)
	{
		return g_pEngineInstance->RequestSkinningVerts(mesh);
	}
	// IN: Mesh*        - pointer to the mesh releasing verts
	//     unsigned int - first offset of the lsit
	// OUT: void
	//
	// Releases the vertices the mesh is writing so other meshes may use them
	void ReleaseSkinningVerts(Mesh* mesh, unsigned int firstOffset)
	{
		g_pEngineInstance->ReleaseSkinningVerts(mesh, firstOffset);
	}

	void SetWindowStyle(unsigned int style)
	{
		g_pEngineInstance->SetWindowStyle(style);
	}
	void SetWindowDimensions(float width, float height)
	{
		g_pEngineInstance->SetWindowDimensions(width, height);
	}
	void SetVSync(bool vsync)
	{
		g_pEngineInstance->SetVSync(vsync);
	}
	void SetAnisotropicFiltering(unsigned int anisotropic)
	{
		g_pEngineInstance->SetAnisotropicFiltering(anisotropic);
	}
	void SetGamma(float gamma)
	{
		g_pEngineInstance->SetGamma(gamma);
	}

	Vector2 GetWindowDimensions()
	{
		return g_pEngineInstance->GetWindowDimensions();
	}
	unsigned int GetWindowStyle()
	{
		return g_pEngineInstance->GetWindowStyle();
	}
	bool IsWindowDirty()
	{
		return g_pEngineInstance->IsWindowDirty();
	}
	unsigned int GetNumDisplays()
	{
		return g_pEngineInstance->GetNumDisplays();
	}
	void GetSupportedResolutions(std::vector<std::wstring>& resolutions, unsigned int displayID)
	{
		g_pEngineInstance->GetSupportedResolutions(resolutions, displayID);
	}
	bool VSyncEnabled()
	{
		return g_pEngineInstance->VSyncEnabled();
	}
	unsigned int GetAnisotropicFiltering()
	{
		return g_pEngineInstance->GetAnisotropicFiltering();
	}
	float GetGamma()
	{
		return g_pEngineInstance->GetGamma();
	}

#if defined(DEBUG) || defined(_DEBUG)
	void StartEvent(const wchar_t* name)
	{
		g_pEngineInstance->StartEvent(name);
	}
	void EndEvent()
	{
		g_pEngineInstance->EndEvent();
	}
#endif
}

namespace InfernoEngine_Interface
{
	void SetEngineInstance(InfernoEngine* _engine)
	{
		g_pEngineInstance = _engine;
	}
}

namespace InfernoEngine_Input
{
	// === RegisterEventToActionBinding
	//  IN : char * - The name of the Action Binding
	//       ActionEventFunction - The delegate fuction
	//  OUT : void
	//  
	//  Registers the given delegate function to the given Action Binding, which will get called when that binding is triggered.
	void RegisterEventToActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _delegate)
	{
		InputManager::RegisterEventToActionBinding(_binding, _callingObject, _delegate);
	}

	// === RegisterEventToAxisBinding
	//  IN : char* - The name of the Axis Binding
	//       AxisEventFunction - The delegate function
	//  OUT : void
	//
	//  Registers the given delegate function to the given Axis Binding, which will get called with each check of the axis.
	void RegisterEventToAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _delegate)
	{
		InputManager::RegisterEventToAxisBinding(_binding, _callingObject, _delegate);
	}

	// === RegisterEventToMouseMovement
	//  IN : MouseMovementEventFunction - The delegate function
	//  OUT : void
	//
	//  Registers the given delegate function to the Mouse Movement, which will get called anytime the mouse is moved.
	void RegisterEventToMouseMovement(void* _callingObject, MouseMovementEventFunction _delegate)
	{
		InputManager::RegisterEventToMouseMovement(_callingObject, _delegate);
	}

	// === UnregisterEventToActionBinding
	//  IN : char* - The name of the Action Binding
	//       ActionEventFunction - The delegate function to unregister
	//  OUT : bool - Whether or not the delegate was found and registered
	//
	//  Unregisters the given delegate function from the Action Binding, if it exists. Returns true if it was found and unregistered,
	//  false other wise.
	bool UnregisterEventFromActionBinding(const char* _binding, void* _callingObject, ActionEventFunction _delegate)
	{
		return InputManager::UnregisterEventFromAction(_binding, _callingObject, _delegate);
	}

	// === UnregisterEventFromActionBinding
	//  IN : unsigned int - The hash key of the Action Binding
	//       ActionEventFunction - The delegate function to unregister
	//  OUT : bool - Whether or not the delegate was found and registered
	//
	//  Unregisters the given delegate function from the Action Binding, if it exists. Returns true if it was found and unregistered,
	//  false other wise.
	bool UnregisterEventFromActionBinding(unsigned int _hash, void* _callingObject, ActionEventFunction _delegate)
	{
		return InputManager::UnregisterEventFromAction(_hash, _callingObject, _delegate);
	}

	// === UnregisterEventFromAxisBinding
	//  IN : char* - The name of the Axis Binding
	//       AxisEventFunction - The delegate function to unregister
	//  OUT : bool - Whether or not the delegate was foud and registered
	//
	//  Unregisters the given delegate function from the Axis Binding, if it exists. Returns true if it was found and unregistered,
	//  false other wise.
	bool UnregisterEventFromAxisBinding(const char* _binding, void* _callingObject, AxisEventFunction _delegate)
	{
		return InputManager::UnregisterEventFromAxis(_binding, _callingObject, _delegate);
	}

	// === UnregisterEventFromAxisBinding
	//  IN : unsigned int - The hash key of the Axis Binding
	//       AxisEventFunction - The delegate function to unregister
	//  OUT : bool - Whether or not the delegate was found and unregistered
	//
	//  Unregisters the given delegate function from the Axis Binding, if it exists. Returns true if it was found and unregistered,
	//  false other wise.
	bool UnregisterEventFromAxisBinding(unsigned int _hash, void* _callingObject, AxisEventFunction _delegate)
	{
		return InputManager::UnregisterEventFromAxis(_hash, _callingObject, _delegate);
	}

	// === UnregisterEventFromMouseMovement
	//  IN : MouseMovementEvent - The delegate function to unregister
	//  OUT : bool - Whether or not the delegate was found and unregistered
	//
	//  Unreigsters the given delegate function from the MouseMovement Event, if it exists. Returns true if it was found and 
	//  unregistered, false otherwise.
	bool UnregisterEventFromMouseMovement(void* _callingObject, MouseMovementEventFunction _delegate)
	{
		return InputManager::UnregisterEventFromMouseMovement(_callingObject, _delegate);
	}
}

namespace InfernoEngine_ObjectManagement
{
	// === RegisterGameObject
	//  IN : GameObject* - Pointer to the GameObject
	//  OUT : void
	//
	//  Registers a GameObject with the ObjectManager's master list of GameObjects, which handles cleanup at the shutdown of the engine
	void RegisterGameObject(GameObject* _gameObject)
	{
		g_pEngineInstance->RegisterGameObject(_gameObject);
	}

	// === RemoveFromSceneList
	//  IN : GameObject* - Pointer to the GameObject
	//  OUT : void
	// 
	//  Removes a GameObject from the ObjectManager's list of objects in the Scene. This doesn't mean that the object is being destroyed, just 
	//  stops the ObjectManager from calling that GameObjects update directly, this should happen when a GameObject gets parented to another, 
	//  as the parent handles calling the child's update.
	void RemoveFromSceneList(GameObject* _gameObject)
	{
		g_pEngineInstance->RemoveFromSceneList(_gameObject);
	}

	// === TrueDestroy
	//  IN : GameObject* - Pointer to the GameObject to Destroy
	//  OUT : void
	//
	//  Destroys a GameObject this exact frame, by bypassing the PendingDestroy list. Should only be called by the GameObject's destructor to handle
	//  deleting it's children at the same time it gets destroyed.
	void TrueDestroy(GameObject* _gameObject)
	{
		g_pEngineInstance->TrueDestroy(_gameObject);
	}

	bool CreatePrefab(Prefab* _prefabData, unsigned int _allocationAmount)
	{
		return g_pEngineInstance->CreatePrefab(_prefabData, _allocationAmount);
	}

	IComponent* CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform)
	{
		return g_pEngineInstance->CreateComponent(_id, _gameObject, _transform);
	}

	// === AddForOnEnable
	//  IN : GameObject* - Pointer to the GameObject that was just enabled
	//  OUT : void
	//
	//  Adds a newly enabled GameObject to the ObjectManager's PendingOnEnableList so that the object's OnEnable is called next frame, after any values
	//  that might have been changed are done.
	void AddForOnEnable(GameObject* _gameObject)
	{
		g_pEngineInstance->AddForOnEnable(_gameObject);
	}

	// === RemoveFromOnEnable
	//  IN : GameObject* - Pointer to the GameObject to remove from the PendingOnEnable List
	//  OUT : void
	//
	//  Removes a GameObject from the PendingOnEnable list in the ObjectManager. This should be used in cases where it's possible that the GameObject might
	//  get called twice for OnEnable, such as if it's parent is also in that list.
	void RemoveFromOnEnable(GameObject* _gameObject)
	{
		g_pEngineInstance->RemoveFromOnEnable(_gameObject);
	}

	// === AddForOnDisable
	//  IN : GameObject* - Pointer to the GameObject that was just disabled
	//  OUT : void
	//
	//  Adds a newly disabled GameObject to the ObjectManager's PendingOnDisableList so that is called next frame, after any other changes.
	void AddForOnDisable(GameObject* _gameObject)
	{
		g_pEngineInstance->AddForOnDisable(_gameObject);
	}
}

namespace InfernoEngine_Physics
{
	// === RegisterColliderComponent
	//  IN : Collider* - The collider to register
	//  OUT : bool - Whether or not the collider was able to register
	//
	//  Registers the given Collider with the Physics Module. Returns true if it was able to register, false otherwise.
	bool RegisterColliderComponent(Collider* _collider)
	{
		return g_pEngineInstance->RegisterCollider(_collider);
	}

	// === RegisterRigidbodyComponent
	//  IN : Rigidbody* - The rigidbody to register
	//  OUT : bool - Whether or not the rigidbody was able to register
	//
	//  Register the given Rigidbody with the Physics Module. Returns true if it was able to register, false otherwise.
	bool RegisterRigidbodyComponent(Rigidbody* _rigidbody)
	{
		return g_pEngineInstance->RegisterRigidBody(_rigidbody);
	}

	// === UnregisterColliderComponent
	//  IN : Collider* - The pointer to the Collider to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given Collider from the Physics Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterColliderComponent(Collider* _collider)
	{
		return g_pEngineInstance->UnregisterColliderComponent(_collider);
	}

	// === UnregisterRigidbodyComponent
	//  IN : Rigidbody* - The pointer to the Rigidbody to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given Rigidbody from the Physics Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterRigidbodyComponent(Rigidbody* _rigidbody)
	{
		return g_pEngineInstance->UnregisterRigidbodyComponent(_rigidbody);
	}

	//  IN : Ray - The ray to check collision against
	//  OUT : RayCastHit - information about what the ray hit
	//
	//	returns true if the ray hit a collider, false if it didn't
	bool RayCast(Ray& ray, RayCastHit& hitInfo){
		return g_pEngineInstance->RayCast(ray, hitInfo);
	}

	// IN: origin - the origin of the sphere
	//	   radius - the radius of the sphere
	// OUT: hitinfo - vector of information about what the sphere overlapped with
	// 
	// Checks to see if a ray collides with an object
	bool SphereOverlap(const Vector3& origin, const float radius, std::vector<RayCastHit>& hitInfo){
		return g_pEngineInstance->SphereOverlap(origin, radius, hitInfo);
	}
}

namespace InfernoEngine_UI
{
	// === RegisterUIEventSystem
	//  IN : UIEventSystem* - The eventsystem to register
	//  OUT : bool - Whether or not the eventsystem was able to register
	// 
	//  Registers the given UIEventSystem with the UIManager. Returns true if it was able to register, false otherwise.
	bool RegisterUIEventSystem(UIEventSystem* _eventSystem)
	{
		return g_pEngineInstance->RegisterUIEventSystem(_eventSystem);
	}

	// === UnregisterUIEventSystem
	//  IN : UIEventSystem - The pointer to the UIEventSystem to unregister
	//  OUT : Whether or not it was unregistered
	//
	//  Unregisters the given UIEventSystem from the UIManager. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterUIEventSystem(UIEventSystem* _eventSystem)
	{
		return g_pEngineInstance->UnregisterUIEventSystem(_eventSystem);
	}

	// === RegisterForCanvasScaleChange
	//  IN : string& - Name of the Canvas to register with
	//       void* - The owning object of the function to be called.
	//       CanvasScaleChangeFunc - Func to be registered. Format of void(void)
	//  OUT : void
	//
	//  Registers a Function to be called when a specific canvas changes it's scale size. 
	void RegisterForCanvasScaleChange(std::string& _canvasName, void* _callingObject, std::function<void(void)> _func)
	{
		g_pEngineInstance->RegisterForCanvasScaleChange(_canvasName, _callingObject, _func);
	}

	// === UnregisterFromCanvasScaleChange
	//  IN : string& - Name of the Canvas to unregister from
	//       void* - The owning object of the function that was registered.
	//  OUT : void
	// 
	//  Unregisters a CanvasScaleChange Function from a specific canvas.
	void UnregisterFromCanvasScaleChange(std::string& _canvasName, void* _callingObject)
	{
		g_pEngineInstance->UnregisterFromCanvasScaleChange(_canvasName, _callingObject);
	}
}

namespace InfernoEngine_SceneManagement{

	void LoadScene(const char* sceneName){
		g_pEngineInstance->SceneChange(sceneName);
	}
}



