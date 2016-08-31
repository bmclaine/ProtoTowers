// STRAWMAN
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>
#include <future>
#include "MathLib.h"
#include "ObjectRegistry.h"

class InfernoEngine;
class GameObject;
class Mesh;
class UICanvas;
class UIElement;
class NavAgent;
class Animation;
class AnimationController;
class UISelectable;
class AIEntity;
class WorkerThread;

struct Material;

namespace Inferno
{
	void InitializeEngine(HINSTANCE hInstance, LPCWSTR IconName = nullptr, LPCWSTR SmallIconName = nullptr, LPCWSTR className = nullptr);

	bool UpdateEnginePreAssets();
	bool UpdateEngine();

	void DestroyEngine();

	void ExitApp();

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

	// These should be temp and removed when the scene manager is set up to auto set these per scene
	//----------------------------------------------------------------------------------------------
	GameObject* GetNewGameObject();

	int GetEffectByName(const char* name);

	void RegisterAudioListener(const GameObject* gameObject);

	void UnregisterAudioListener(const GameObject* gameObject);

	Mesh* GetMeshByName(const char* name);

	int GetImage(const wchar_t* name);

	Material* GetMaterial(std::string& _materialName);

	void PostAudioEvent(unsigned int aEvent);

	void RegisterNavAgent(NavAgent* _agent);

	void UnRegisterNavAgent(NavAgent* _agent);

	void RegisterAIEntity(AIEntity* _entity);

	void UnregisterAIEntity(AIEntity* _entity);

	void RegisterPlayerToAIModule(GameObject* _player);

	void UnregisterPlayerToAIModule(GameObject* _player);

	void InitializeNavMesh(Mesh* _navMesh, Matrix4& _globalPosition);

	Animation* GetAnimation(const char* name);

	AnimationController* GetAnimationControllerByName(const char* name);
	
	void RegisterObjectToAudioManager(GameObject * _object);
	
	void UnregisterObjectToAudioManager(GameObject * _object);

	void PostAudioEventAtObjectPosition(unsigned int aEvent, GameObject * _object);

	void StopAllAudio();
	//----------------------------------------------------------------------------------------------
	// End Temp Functions

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

	bool AllocatePrefab(std::string _prefabName, GameObject* _basePrefab, unsigned int _amount);

	// IN: void
	// OUT: void
	//
	// Launches a seperate thread to load all the assets the game needs to run
	void* LoadAssets(std::future<bool>* future);

	// IN: void
	// OUT: void
	//
	// Resets the Time class
	void ResetTime();

	void ClearScene();

	// IN: void
	// OUT: void
	//
	// Destroys all active particles
	void CleanUpParticles();

	// === GetGameObjectByName
	//  IN : string - Name of the desired GameObject
	//  OUT : GameObject* - The Found GameObject
	//
	//  Finds a GameObject with the given name that is in the current Scene, if one exists. Returns nullptr if no GameObject was found with that name.
	GameObject* GetGameObjectByName(std::string& _gameObjectName);

	// IN: WorkerThread* - Pointer to work being added **NOTE** - This must be dynamic memory or locally stored till done
	// OUT: void
	//
	// Adds work to the worker thread queue
	void PushWork(WorkerThread* work);

	void SetAmbientColor(float r, float g, float b);
	void SetAmbientColor(Vector3 color);
	void SetAmbientColor(Vector4 color);
	void SetAmbientIntensity(float intensity);

	// IN: vector<pair<string, string>> - First string is name to update, Second string is value to replace
	// OUT: void
	//
	// Updates the config file parameters that are requested with the updated values
	void UpdateConfigFile(std::vector<std::pair<std::string, std::string>>& parameters);
}

namespace Inferno_Components
{
	template <typename type>
	static unsigned int RegisterComponent(const char* _componentID) {
		return ObjectRegistry::RegisterComponentType<type>(_componentID);
	}

	template <typename type>
	static unsigned int RegisterUIComponent(const char* _uiComponentID) {
		return ObjectRegistry::RegisterUIComponentType<type>(_uiComponentID);
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
	void CreateNewCanvas(std::string _canvasName, UICanvas** _outCreatedCanvas, unsigned int* _canvasSlot);
	// === DestroyCanvas
	//  IN : string - Name of the Canvas
	//  OUT : void
	//
	//  Destroys a Canvas and all UIElements that are attached to the Canvas.
	void DestroyCanvas(std::string _canvasName);
	// === GetCanvasByName
	//  IN : string - The name of the Canvas
	//  OUT : UICanvas* - The desired Canvas, if it exists
	//
	//  Returns the Canvas found under the given name, if it exists.
	UICanvas* GetCanvasByName(std::string _canvasName);
	// === AddToView
	//  IN : UIElement* - Pointer to the UIElement to add
	//       unsigned int - The slot of the canvas to add the element to
	//  OUT : Bool - Whether or not the element was added
	//
	//  Adds an UIElement to the desired Canvas, if that canvas is valid. Returns true if the element was added, false otherwise.
	bool AddToView(UIElement* _element, unsigned int _canvasSlot);
	// === AddToView
	//  IN : UIElement* - The Pointer to the UIElement to add
	//       std::string - The name of the Canvas to add the element to
	//  OUT : bool - Whether or not the element was added to the desired canvas
	//
	//  Adds the given UIElement to a desired UICanvas. Returns true if the canvas was found and the element was added, false otherwise.
	bool AddToView(UIElement* _element, std::string _canvasName);
	// === SetTargetResolution
	//  IN : float - The target width
	//       float - The target height
	//  OUT : void 
	//
	//  Sets the Target Resolution that UI is designed for.
	void SetTargetResolution(float _width, float _height);
	// === SetCurrentSelectable
	//  IN : UISelectable* - Pointer to the Selectable that should be focused on
	//  OUT : void
	//
	//  Sets the UISelectable that the UIController should be focusing on
	void SetCurrentSelectable(UISelectable* _selectable);
	// === GetUIElementByName
	//  IN : string - Name of the Element to look for
	//       string - (Optional) Name of the Canvas to look in
	//  OUT : UIElement* - The found UIElement
	//
	//  Looks for a UIElement with the given name, on the specified canvas, if one is specified. Returns the found element, or nullptr if no element
	//  was found with that name
	UIElement* GetUIElementByName(std::string& _elementName, std::string& _canvasName = std::string());
	// === GetCurrentSelectable
	//  IN : void
	//  OUT : UISelectable* - Pointer to the Selectable that is selected, nullptr if none
	//
	//  Returns the currently selected UISelectable
	UISelectable* GetCurrentSelectable();

	void ClearView();

	// === IsFocusedElementLocked
	//  IN : void
	//  OUT : bool - Whether or not the current focused element is locked
	//
	//  Checks to see if the current focused element is locked or not.
	bool IsFocusedElementLocked();
}