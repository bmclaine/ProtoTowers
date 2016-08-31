#include "../Public/Engine.h"
#include "InfernoEngine.h"
#include "../Public/ModuleInterface.h"
#include "Components\NavAgent.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "../../Interface/Public/ComponentInterface.h"

InfernoEngine* engine = nullptr;

namespace Inferno
{
	void InitializeEngine(HINSTANCE hInstance, LPCWSTR IconName, LPCWSTR SmallIconName, LPCWSTR className)
	{
		if (!engine)
		{
			engine = new InfernoEngine();
			InfernoEngine_Interface::SetEngineInstance(engine);

			engine->Initialize(hInstance, IconName, SmallIconName, className);

			InfernoEngine_Components::ComponentInterface::SetEngineInstance(engine);
		}
	}

	bool UpdateEnginePreAssets()
	{
		if (engine)
			return engine->UpdatePreAsset();

		return false;
	}
	bool UpdateEngine()
	{
		if (engine)
			return engine->Update();

		return false;
	}

	void DestroyEngine()
	{
		if (engine)
		{
			engine->Destroy();
			delete engine;
			engine = nullptr;
		}
	}

	void ExitApp()
	{
		if (engine)
			engine->Quit();
	}

	// IN: char* - Name of the effect
	//     int   - ID of the pixel shader
	//     int   - ID of the rasterizer state
	//     int   - ID of the blend state
	//     int*  - List of SamplerState IDs
	//     int   - Count of the sampler states
	// OUT: int - ID of the added effect
	//
	// Creates an effect and adds it to the list of effects if it doesn't already exits and returns the ID
	int CreateEffect(const char* name, int pixelShader, int rasterizerState, int blendState, int* samplerStates, int samplerCount)
	{
		if (engine)
			return engine->CreateEffect(name, pixelShader, rasterizerState, blendState, samplerStates, samplerCount);

		return -1;
	}

	// These should be temp and removed when the scene manager is set up to auto set these per scene
	//----------------------------------------------------------------------------------------------
	GameObject* GetNewGameObject()
	{
		if (engine)
			return engine->GetNewGameObject();

		return nullptr;
	}

	int GetEffectByName(const char* name)
	{
		if (engine)
			return engine->GetEffectByName(name);

		return -1;
	}

	void RegisterAudioListener(const GameObject* gameObject)
	{
		if (engine)
			engine->RegisterAudioListener(gameObject);
	}

	void UnregisterAudioListener(const GameObject* gameObject)
	{
		if (engine)
			engine->UnregisterAudioListener(gameObject);
	}

	Mesh* GetMeshByName(const char* name)
	{
		if (engine)
			return engine->GetMeshByName(name);

		return nullptr;
	}

	Material* GetMaterial(std::string& _materialName){
		if (engine)
			return engine->GetMaterial(_materialName);

		return nullptr;
	}


	int GetImage(const wchar_t* name)
	{
		if (engine)
			return engine->GetImage(name);

		return -1;
	}

	void PostAudioEvent(unsigned int aEvent)
	{
		if (engine)
			engine->PostAudioEvent(aEvent);
	}
	void RegisterNavAgent(NavAgent* _agent)
	{
		if (engine)
			engine->RegisterNavAgent(_agent);
	}
	void UnRegisterNavAgent(NavAgent* _agent)
	{
		if (engine)
			engine->UnregisterNavAgent(_agent);
	}
	void RegisterPlayerToAIModule(GameObject* _player)
	{
		if (engine)
			engine->RegisterPlayer(_player);
	}
	void UnregisterPlayerToAIModule(GameObject* _player)
	{
		if (engine)
			engine->UnregisterPlayer(_player);
	}
	void RegisterAIEntity(AIEntity* _entity)
	{
		if (engine)
			engine->RegisterAIEntity(_entity);
	}
	void UnregisterAIEntity(AIEntity* _entity)
	{
		if (engine)
			engine->UnregisterAIEntity(_entity);
	}
	void InitializeNavMesh(Mesh* _navMesh, Matrix4& _matrix)
	{
		if (engine)
			engine->RegisterNavMesh(_navMesh, _matrix);
	}
	Animation* GetAnimation(const char* name)
	{
		if (engine)
			return engine->GetAnimation(name);

		return nullptr;
	}
	AnimationController* GetAnimationControllerByName(const char* name)
	{
		if (engine)
			return engine->GetAnimationControllerByName(name);

		return nullptr;
	}
	void RegisterObjectToAudioManager(GameObject * _object)
	{
		if (engine)
			engine->RegisterObjectToAudioManager(_object);
	}
	void UnregisterObjectToAudioManager(GameObject * _object)
	{
		if (engine)
			engine->UnregisterObjectToAudioManager(_object);
	}

	void PostAudioEventAtObjectPosition(unsigned int aEvent, GameObject * _object)
	{
		if (engine)
			engine->PostAudioEventAtObjectPosition(aEvent, _object);
	}

	void StopAllAudio()
	{
		if (engine)
			engine->StopAllAudio();
	}
	//----------------------------------------------------------------------------------------------
	// End Temp Functions

	// === Instaniate
	//  IN : string - The name of the Prefab that you want back. Can be left blank if you want just a new GameObject.
	//  OUT : GameObject* - Pointer to an instance of the Prefab you requested, or a blank GameObject.
	//
	//  Allocates and returns an instance of the Prefab that you request, if it exists. If it doesn't exist, or the parameter is left blank
	//  then it just returns a new blank GameObject.
	GameObject* Instantiate(std::string _prefabName)
	{
		return engine->Instantiate(_prefabName);
	}

	// === Destroy
	//  IN : GameObject* - Pointer to the GameObject that should be destroyed
	//       float - (Optional) Delay time to destroy the GameObject
	//  OUT : void
	//
	//  Removes 
	void Destroy(GameObject* _gameObject, float _delayTime)
	{
		engine->Destroy(_gameObject, _delayTime);
	}

	bool AllocatePrefab(std::string _prefabName, GameObject* _basePrefab, unsigned int _amount)
	{
		return engine->AllocatePrefab(_prefabName, _basePrefab, _amount);
	}

	void* LoadAssets(std::future<bool>* future){
		if (engine)
			return engine->LoadAssets(future);

		return nullptr;
	}

	// IN: void
	// OUT: void
	//
	// Resets the Time class
	void ResetTime()
	{
		if (engine)
			engine->ResetTime();
	}

	void ClearScene()
	{
		engine->ClearScene();
	}

	// IN: void
	// OUT: void
	//
	// Destroys all active particles
	void CleanUpParticles()
	{
		engine->CleanUpParticles();
	}

	// === GetGameObjectByName
	//  IN : string - Name of the desired GameObject
	//  OUT : GameObject* - The Found GameObject
	//
	//  Finds a GameObject with the given name that is in the current Scene, if one exists. Returns nullptr if no GameObject was found with that name.
	GameObject* GetGameObjectByName(std::string& _gameObjectName)
	{
		return engine->GetGameObjectByName(_gameObjectName);
	}

	// IN: WorkerThread* - Pointer to work being added **NOTE** - This must be dynamic memory or locally stored till done
	// OUT: void
	//
	// Adds work to the worker thread queue
	void PushWork(WorkerThread* work)
	{
		engine->PushWork(work);
	}

	void SetAmbientColor(float r, float g, float b)
	{
		engine->SetAmbientColor(r, g, b);
	}
	void SetAmbientColor(Vector3 color)
	{
		engine->SetAmbientColor(color);
	}
	void SetAmbientColor(Vector4 color)
	{
		engine->SetAmbientColor(color);
	}
	void SetAmbientIntensity(float intensity)
	{
		engine->SetAmbientIntensity(intensity);
	}

	// IN: vector<pair<string, string>> - First string is name to update, Second string is value to replace
	// OUT: void
	//
	// Updates the config file parameters that are requested with the updated values
	void UpdateConfigFile(std::vector<std::pair<std::string, std::string>>& parameters)
	{
		engine->UpdateConfigFile(parameters);
	}
}

namespace Inferno_UI
{
	// === CreateNewCanvas
	//  IN : UICanvas* - { Out Parameter } Pointer to the Created UICanvas
	//       unsigned int* - { Out Parameter } Pointer to the slot number that the created canvas was put in.
	//  OUT : void
	//
	//  Creates a new UICanvas for UIElements to be added to. Returns the UICanvas, and the canvas slot through parameters.
	void CreateNewCanvas(std::string _canvasName, UICanvas** _createdCanvas, unsigned int* _canvasSlot)
	{
		engine->CreateNewUICanvas(_canvasName, _createdCanvas, _canvasSlot);
	}

	// === DestroyCanvas
	//  IN : string - Name of the Canvas
	//  OUT : void
	//
	//  Destroys a Canvas and all UIElements that are attached to the Canvas.
	void DestroyCanvas(std::string _canvasName)
	{
		engine->DestroyCanvas(_canvasName);
	}

	// === GetCanvasByName
	//  IN : string - The name of the Canvas
	//  OUT : UICanvas* - The desired Canvas, if it exists
	//
	//  Returns the Canvas found under the given name, if it exists.
	UICanvas* GetCanvasByName(std::string _canvasName)
	{
		return engine->GetCanvasByName(_canvasName);
	}

	// === AddToView
	//  IN : UIElement* - Pointer to the UIElement to add
	//       unsigned int - The slot of the canvas to add the element to
	//  OUT : Bool - Whether or not the element was added
	//
	//  Adds an UIElement to the desired Canvas, if that canvas is valid. Returns true if the element was added, false otherwise.
	bool AddToView(UIElement* _element, unsigned int _canvasSlot)
	{
		return engine->AddToView(_element, _canvasSlot);
	}

	// === AddToView
	//  IN : UIElement* - The Pointer to the UIElement to add
	//       std::string - The name of the Canvas to add the element to
	//  OUT : bool - Whether or not the element was added to the desired canvas
	//
	//  Adds the given UIElement to a desired UICanvas. Returns true if the canvas was found and the element was added, false otherwise.
	bool AddToView(UIElement* _element, std::string _canvasName)
	{
		return engine->AddToView(_element, _canvasName);
	}

	// === SetTargetResolution
	//  IN : float - The target width
	//       float - The target height
	//  OUT : void 
	//
	//  Sets the Target Resolution that UI is designed for.
	void SetTargetResolution(float _width, float _height)
	{
		engine->SetUITargetResolution(_width, _height);
	}

	// === SetCurrentSelectable
	//  IN : UISelectable* - Pointer to the Selectable that should be focused on
	//  OUT : void
	//
	//  Sets the UISelectable that the UIController should be focusing on
	void SetCurrentSelectable(UISelectable* _selectable)
	{
		engine->SetCurrentSelectable(_selectable);
	}

	// === GetUIElementByName
	//  IN : string - Name of the Element to look for
	//       string - (Optional) Name of the Canvas to look in
	//  OUT : UIElement* - The found UIElement
	//
	//  Looks for a UIElement with the given name, on the specified canvas, if one is specified. Returns the found element, or nullptr if no element
	//  was found with that name
	UIElement* GetUIElementByName(std::string& _elementName, std::string& _canvasName)
	{
		return engine->GetUIElementByName(_elementName, _canvasName);
	}

	// === GetCurrentSelectable
	//  IN : void
	//  OUT : UISelectable* - Pointer to the Selectable that is selected, nullptr if none
	//
	//  Returns the currently selected UISelectable
	UISelectable* GetCurrentSelectable()
	{
		return engine->GetCurrentSelectable();
	}

	void ClearView()
	{
		engine->ClearView();
	}

	// === IsFocusedElementLocked
	//  IN : void
	//  OUT : bool - Whether or not the current focused element is locked
	//
	//  Checks to see if the current focused element is locked or not.
	bool IsFocusedElementLocked()
	{
		return engine->IsFocusedElementLocked();
	}
}