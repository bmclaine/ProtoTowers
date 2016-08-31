// Author: Jonathan Gyurkovics
//
// The core engine that is responsible for keeping track of all the major components. It will handle the creation calls and destroy calls.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <atomic>
#include <mutex>
#include <future>

#include "Win32Window\IWndProc.h"

#include "../../Utilities/Public/MathLib.h"
#include "../../Utilities/Public/ShaderInfo.h"
#include "../../Utilities/Public/InfernoTimer.h"
#include "AudioManager.h"
#include "ObjectManager.h"

#include "Color.h"

// Managers
class GraphicsManager;
class CameraManager;
class Physics;
class AssetManager;
class FontManager;
class UIManager;
class AIModule;
class SceneManager;
class WorkerThreadPool;

// Non-managers
class GameObject;
class Renderer;
class SpriteRenderer;
class Animation;
class AnimationController;
class NavAgent;
class Rigidbody;
class Collider;
class Light;
class Font;
class Mesh;
class ParticleSystem;
class UIElement;
class UIEventSystem;
class UIRenderer;
class UIText;
class UICanvas;
class UISelectable;
class AIEntity;
class Transform;
class WorkerThread;
class WorkerThreadFuctionPointers;

struct Ray;
struct RayCastHit;
struct Material;
class Texture;

#define SAFE_RELASE(x) if(x) { x->Release(); x = nullptr; }

// An enum that can be used to determine the state of a current manager
enum MANAGER_STATUS { 
	MANAGER_STATUS_UNINITALIZED,
	MANAGER_STATUS_INITIALIZED,
	MANAGER_STATUS_DESTORYED,
	MANAGER_STATUS_ERROR,
	MANAGER_STATUS_QUIT,
};

class InfernoEngine : public IWndProc
{
private:
	MANAGER_STATUS m_eEngineStatus;		// Holds the current status of the engine
	MSG m_msg;							// The current system message

	GraphicsManager*  m_pGraphicsManager;		// Holds a pointer to the graphics manager instance
	AudioManager      m_AudioManager;
	Physics*		  m_pPhysics;
	ObjectManager*    m_pObjectManager;
	CameraManager*    m_pCameraManager;
	AssetManager*     m_pAssetManager;
	FontManager*      m_pFontManager;			// Holds a pointer to the font manager instance
	UIManager*        m_pUIManager;
	AIModule*         m_pAIModule;
	SceneManager*	  m_pSceneManager;
	WorkerThreadPool* m_pWorkerThreads;

	std::mutex		  m_mConfigMutex;

	// Statistics Info data
	InfernoTimer timer;
	std::string m_sCanvasName;
	UICanvas* m_pCanvas;
	Texture* m_pStatisticsBackground;
	UIText* m_pFPSText;
	DWORD m_uiFrameCount;
	DWORD m_uiFrameRate;
	bool m_bDisplayFPS;

#if defined(DEBUG) || defined(_DEBUG)
	bool m_bDisplayGizmos;
#endif

	void AssetLoader();
	void LoadingFinished();

	// Remove our copy ctor and assignement operator
	InfernoEngine(const InfernoEngine&) = delete;
	InfernoEngine& operator=(const InfernoEngine&) = delete;

	LRESULT WndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) override;

public:
	// Ctor and Dtor
	InfernoEngine();
	~InfernoEngine();

	// IN: HINSTANCE - Instance of the application
	//     LPCWSTR   - String holding the Icon resource ID  *OPTIONAL*
	//     LPCWSTR   - String holding the SmallIcon resource ID  *OPTIONAL*
	//     LPCWSTR   - String holding the name of the application  *OPTIONAL*
	//     LPCWSTR   - String for the class name  *OPTIONAL*
	// OUT: bool - States if the manager intialized correctly
	//
	// Initializes the engine and any magangers it will use
	bool Initialize(HINSTANCE hInstance, LPCWSTR IconName = nullptr, LPCWSTR SmallIconName = nullptr, LPCWSTR className = nullptr);
	// IN: void
	// OUT: bool - States if the engine destroyed successfully
	//
	// Destroys the engine and any manager it uses
	bool Destroy();
	// IN: void
	// OUT: void
	//
	// Informs the engine we want to quit
	void Quit();

	// IN: void
	// OUT: void
	//
	// Loads from the config file
	void LoadConfig();

	// IN: vector<pair<string, string>> - First string is name to update, Second string is value to replace
	// OUT: void
	//
	// Updates the config file parameters that are requested with the updated values
	void UpdateConfigFile(std::vector<std::pair<std::string, std::string>>& parameters);

	// IN: void
	// OUT: bool - States if the engine updated successfully
	//
	// Updates only a few components during primary asset loading
	bool UpdatePreAsset();

	// IN: void
	// OUT: bool - States if the engine updated successfully
	//
	// Updates all components of the engine
	bool Update();

	// IN: void
	// OUT: float - the aspect ratio
	//
	// Returns the current aspect ratio
	float GetAspectRatio() const;

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

	// IN: wchar_t* - name of the texture to load
	// OUT: int - index into the texture array
	//
	// Loads a texture if it hasn't been loaded yet and returns the index into the textures array
	int LoadTexture(const wchar_t* filename);
	// IN: wchar_t* - Name of hte texture to store in memory
	// OUT: void
	//
	// Loads the texture file into memory to be loaded onto the GPU at a later point. This automatically
	// queues the texture file for loading and should be used by Threads.
	void StoreTextureInMemory(const wchar_t* filename);

	// IN: char* - Name of scene to load
	// OUT: void
	//
	// Informs the SceneManager that we would like to change scenes when possible
	void SceneChange(const char* sceneName);

	// IN: Mesh* - Mesh to load
	// OUT: unsigned int - starting index of the mesh
	//
	// Loads the passed in mesh into the graphics card if it hasn't been loaded yet and returns the first index location
	unsigned int LoadMesh(Mesh* mesh);
	// IN: void* - Mesh to queue for loading
	//     bool  - If the mesh should be finalized, in some cases we don't want to as with NavMeshes
	// OUT: void
	//
	// Adds a mesh to the queue to be loaded. This should be used by seperate Threads.
	void QueueLoadMesh(void* mesh, bool finalize);

	void* LoadAssets(std::future<bool>* future);

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
	// Resets the Time class
	void ResetTime();

	// TEMP Until we can find a better way to do this
	//----------------------------------------------
	void PostAudioEvent(unsigned int aEvent);
	void PostAudioEventAtObjectPosition(unsigned int aEvent, GameObject * _object);
	void StopAllAudio();
	void RegisterObjectToAudioManager(GameObject * _object);
	void UnregisterObjectToAudioManager(GameObject * _object);
	void SetMasterVolume(float volume);
	void SetSFXVolume(float volume);
	void SetBackgroundMusicVolume(float volume);
	float GetMasterVolume();
	float GetBackgroundVolume();
	float GetSFXVolume();
	//----------------------------------------------
	// End Temp

	// IN: GameObject - Object to copy
	//     Vector3    - Position of object
	//     Quaternion - Rotation of object
	//     Vector3    - Scale of object
	// OUT: GameObject* - pointer to the newly created object
	//
	// Creates an object based on the incoming GameObject
	GameObject* Instantiate(GameObject* object, Vector3 position = Vector3::Zero(), Quaternion rotation = Quaternion::Identity(), Vector3 Scale = { 1.0f, 1.0f, 1.0f });

	// IN : NavAgent* - Pointer to the NavAgent to register
	// OUT : bool - Success in registering the NavAgent
	//
	// Registers a NavAgent with the AI Module
	bool RegisterNavAgent(NavAgent* _navAgent);

	// IN : AIEntity* - The entity to register
	// OUT : bool - The result
	//
	// Returns true if the register is success
	bool RegisterAIEntity(AIEntity* _entity);

	// IN: GameObject* - Pointer to the Player
	// OUT: bool - True if register is success
	//
	// Registers the player object to the AI Module
	bool RegisterPlayer(GameObject* _player);

	// IN : Mesh* - Pointer to the navMesh
	// OUT : bool - Success
	//
	// Initializes the AIModules NavMesh
	bool RegisterNavMesh(Mesh* _mesh, Matrix4& _globalPosition);

	// IN: Renderer* - Pointer to the renderer to register
	// OUT: bool - Success in registering the renderer
	//
	// Registers a renderer for rendering with the Graphics Manager
	bool RegisterRenderComponent(Renderer* _rComponent);

	// IN : UIRenderer* - Pointer to the UIRenderer
	// OUT : bool - Success in registering the component
	//
	// Registers a UIRenderer with the Graphics Manager
	bool RegisterUIRenderComponent(UIRenderer* _rComponent);

	//  IN : Light* - Pointer to the Light Component to unregistered
	//  OUT : bool - Whether or not it was unregistered
	// 
	//  Unregisters the given Light with the Light Manager. Returns true if it was registered, false otherwise.
	bool UnregisterLightComponent(Light* _lComponent);

	//  IN : Light* - Pointer to the Light Component that needs to be registered
	//  OUT : bool - Whether or not the lComponent was able to be registered
	// 
	//  Registers the given Light with the Light Manager. Returns true if it was registered, false otherwise.
	bool RegisterLightComponent(Light* _lComponent);

	// IN: ParticleSystem* - Pointer to the Particle System Component that needs to be registered
	// OUT: bool - Whether or not the Component was able to be registered
	//
	// Registers the given Particle System with the Particle Manager.
	bool RegisterParticleSystem(ParticleSystem* _pComponent);

	// IN: Rigidbody* - Pointer to the RigidBody to register
	// OUT: bool - Success in registering the RigidBody
	//
	// Registers a RigidBody with the Physics Module
	bool RegisterRigidBody(Rigidbody* rigidBody);

	// IN: Collider* - Pointer to the Collider to register
	// OUT: bool - Success in registering the Collider
	//
	// Registers a Collider with the Physics Module
	bool RegisterCollider(Collider* collider);

	// IN: UIEventSystem* - Pointer to the UIEventSystem to register
	// OUT: bool - Success in registering the UIEventSystem
	//
	// Registers a UIEventSystem with the UIManager
	bool RegisterUIEventSystem(UIEventSystem* eventSystem);

	// IN: GameObject*
	// OUT: void
	//
	// Registers the object as the audio listener
	void RegisterAudioListener(const GameObject* gameObject);

	//  IN :  void
	//  OUT : void
	//
	// Tells the AIModule that the game is in a cutscene
	void EnterCutscene();

	//  IN :  void
	//  OUT : void
	//
	// Tells the AIModule that the game is in a cutscene
	void ExitCutscene();

	// IN : AIEntity* - The entity to unregister
	// OUT : bool - The result
	//
	// Returns true if the register is success
	bool UnregisterAIEntity(AIEntity* _entity);

	//  IN : NavAgent* - Pointer to the NavAgent to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given NavAgent from the AI Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterNavAgent(NavAgent* _navAgent);

	// IN : GameObject* - Pointer to the player object
	// OUT : bool - True if unregistered success
	//
	// Unregisters the player object from the AI Module
	bool UnregisterPlayer(GameObject* _player);

	//  IN : Renderer* - The pointer to the Renderer Component to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given Renderer from the Graphics Manager. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterRenderComponent(Renderer* _rComponent);

	//  IN : UIRenderer* - The pointer to the UIRenderer Component to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given UIRenderer from the Graphics Manager. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterUIRenderComponent(UIRenderer* _rComponent);

	// IN: ParticleSystem* - Pointer to the Particle System Component to unregister
	// OUT: bool - Whether or not the Component was unregistered
	//
	// Uregisters the given Particle System with the Particle Manager.
	bool UnregisterParticleSystem(ParticleSystem* _pComponent);

	//  IN : Collider* - The pointer to the Collider to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given Collider from the Physics Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterColliderComponent(Collider* _collider);

	//  IN : Rigidbody* - The pointer to the Rigidbody to unregister
	//  OUT : bool - Whether or not it was unregistered
	//
	//  Unregisters the given Rigidbody from the Physics Module. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterRigidbodyComponent(Rigidbody* _rigidbody);

	//  IN : UIEventSystem - The pointer to the UIEventSystem to unregister
	//  OUT : Whether or not it was unregistered
	//
	//  Unregisters the given UIEventSystem from the UIManager. Returns true if it was found and unregistered, false otherwise.
	bool UnregisterUIEventSystem(UIEventSystem* _eventSystem);

	// IN: void
	// OUT: void
	//
	// Applies the current pipeline state
	void ApplyPipelineState();

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

	// IN: void
	// OUT: GameObject* - new game object
	//
	// Returns a new game object to use
	GameObject* GetNewGameObject();

	// IN: char*  - name of the mesh to grab
	// OUT: Mesh* - pointer to the mesh
	//
	// Returns the requested mesh
	Mesh* GetMeshByName(const char* name);

	Material* GetMaterial(std::string& _materialName);

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

	// IN: char* - name of the effect
	// OUT: int - ID of the effect
	//
	// Returns the ID of the requested effect
	int GetEffectByName(const char* name);

	// IN: char* - Name of the animation
	// OUT: Animation* - Pointer to the aniation
	//
	// Returns the pointer to the requested animation
	Animation* GetAnimation(const char* name);

	// IN: char* - Name of the animation controller
	// OUT: Animation* - Pointer to the animation controller
	//
	// Returns the pointer to the requested animation controller
	AnimationController* GetAnimationControllerByName(const char* name);

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
	// IN: ShaderType - Shader to apply image to
	//     int        - Image ID
	//     int        - Slot to apply
	// OUT: void
	//
	// Applies the requested image to the requested shader slot
	void SetImage(ShaderType type, int ID, int Slot);
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

	// IN: float - The Target Width
	//     float - The Target Height
	// OUT: void
	//
	// Sets the target resolution that UI will be trying to position itself for.
	void SetUITargetResolution(float _width, float _height);

	// IN: UICanvas* - { Out Parameter } Pointer to the Created UICanvas
	//      unsigned int* - { Out Parameter } Pointer to the slot number that the created canvas was put in.
	// OUT: void
	//
	// Creates a new UICanvas for UIElements to be added to. Returns the UICanvas, and the canvas slot through parameters.
	void CreateNewUICanvas(std::string _canvasName, UICanvas** _createdCanvas, unsigned int* _canvasSlot);

	// IN: string - The name of the Canvas
	// OUT: UICanvas* - The desired Canvas, if it exists
	//
	// Returns the Canvas found under the given name, if it exists.
	UICanvas* GetCanvasByName(std::string _canvasName);

	// IN: UIElement* - Pointer to the UIElement to add
	//      unsigned int - The slot of the canvas to add the element to
	// OUT: Bool - Whether or not the element was added
	//
	// Adds an UIElement to the desired Canvas, if that canvas is valid. Returns true if the element was added, false otherwise.
	bool AddToView(UIElement* _element, unsigned int _canvasSlot);

	// IN: UIElement* - Pointer to the UIElement to add
	//      string - The name of the desired Canvas
	// OUT: Bool - Whether or not the element was added
	//
	// Adds an UIElement to the desired Canvas, if that canvas is valid. Returns true if the element was added, false otherwise.
	bool AddToView(UIElement* _element, std::string _canvasName);

	// IN: UISelectable* - Pointer to the Selectable that should be focused on
	// OUT: void
	//
	// Sets the UISelectable that the UIController should be focusing on
	void SetCurrentSelectable(UISelectable* _selectable);

	// IN: GameObject* - Pointer to the GameObject
	// OUT: void
	//
	// Registers a GameObject with the ObjectManager's master list of GameObjects, which handles cleanup at the shutdown of the engine
	void RegisterGameObject(GameObject* _gameObject);

#if defined(DEBUG) || defined(_DEBUG)
	void StartEvent(const wchar_t* name);
	void EndEvent();
#endif

	// IN: GameObject* - Pointer to the GameObject
	// OUT: void
	// 
	// Removes a GameObject from the ObjectManager's list of objects in the Scene. This doesn't mean that the object is being destroyed, just 
	// stops the ObjectManager from calling that GameObjects update directly, this should happen when a GameObject gets parented to another, 
	// as the parent handles calling the child's update.
	void RemoveFromSceneList(GameObject* _gameObject);

	// === Instaniate
	//  IN : string - The name of the Prefab that you want back. Can be left blank if you want just a new GameObject.
	//  OUT : GameObject* - Pointer to an instance of the Prefab you requested, or a blank GameObject.
	//
	//  Allocates and returns an instance of the Prefab that you request, if it exists. If it doesn't exist, or the parameter is left blank
	//  then it just returns a new blank GameObject.
	GameObject* Instantiate(std::string _prefabName = std::string());

	// === Destroy
	//  IN : GameObject* - Pointer to the GameObject that should be destroyed
	//       float - (Optional) Delay time to destroy the GameObject
	//  OUT : void
	//
	//  Removes 
	void Destroy(GameObject* _gameObject, float _delayTime = 0.0f);

	// IN: string - Name of the Canvas
	// OUT: void
	//
	// Destroys a Canvas and all UIElements that are attached to the Canvas.
	void DestroyCanvas(std::string _canvasName);

	bool AllocatePrefab(std::string _prefabName, GameObject* _basePrefab, unsigned int _amount);

	void ClearScene();

	void UnregisterAudioListener(const GameObject* _gameObject);

	void TrueDestroy(GameObject* _gameObject);

	bool CreatePrefab(Prefab* _prefabData, unsigned int _allocationAmount = 1);

	IComponent* CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform);

	UIElement* GetUIElementByName(std::string& _elementName, std::string& _canvasName = std::string());

	UISelectable* GetCurrentSelectable();

	void AddForOnEnable(GameObject* _gameObject);

	void RemoveFromOnEnable(GameObject* _gameObject);

	void AddForOnDisable(GameObject* _gameObject);

	GameObject* GetGameObjectByName(std::string& _gameObjectName);

	// IN: WorkerThread* - Pointer to work being added **NOTE** - This must be dynamic memory or locally stored till done
	// OUT: void
	//
	// Adds work to the worker thread queue
	void PushWork(WorkerThread* work);

	// IN: void
	// OUT: void
	//
	// Destroys all active particles
	void CleanUpParticles();

	void SetWindowStyle(UINT style);
	void SetWindowDimensions(float width, float height);
	void SetVSync(bool vsync);
	void SetAnisotropicFiltering(unsigned int anisotropic);
	void SetGamma(float gamma);

	Vector2 GetWindowDimensions() const;
	UINT GetWindowStyle() const;
	bool IsWindowDirty() const;
	UINT GetNumDisplays() const;
	void GetSupportedResolutions(std::vector<std::wstring>& resolutions, UINT displayID = 0) const;
	bool VSyncEnabled() const;
	unsigned int GetAnisotropicFiltering() const;
	float GetGamma() const;

	//  IN : Ray - The ray to check collision against
	//  OUT : RayCastHit - information about what the ray hit
	//
	//	returns true if the ray hit a collider, false if it didn't
	bool RayCast(Ray& ray, RayCastHit& hitInfo);

	// IN: origin - the origin of the sphere
	//	   radius - the radius of the sphere
	// OUT: hitinfo - vector of information about what the sphere overlapped with
	// 
	// Checks to see if a ray collides with an object
	bool SphereOverlap(const Vector3& origin, const float radius, std::vector<RayCastHit>& hitInfo);

	// === RegisterForCanvasScaleChange
	//  IN : string& - Name of the Canvas to register with
	//       void* - The owning object of the function to be called.
	//       CanvasScaleChangeFunc - Func to be registered. Format of void(void)
	//  OUT : void
	//
	//  Registers a Function to be called when a specific canvas changes it's scale size. 
	void RegisterForCanvasScaleChange(std::string& _canvasName, void* _callingObject, std::function<void(void)> _func);
	// === UnregisterFromCanvasScaleChange
	//  IN : string& - Name of the Canvas to unregister from
	//       void* - The owning object of the function that was registered.
	//  OUT : void
	// 
	//  Unregisters a CanvasScaleChange Function from a specific canvas.
	void UnregisterFromCanvasScaleChange(std::string& _canvasName, void* _callingObject);

	void ClearView();

	bool IsFocusedElementLocked();
};