//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "stdafx.h"
#include "GlobalIncludes.h"
#include "CutScene.h"

#include "InfernoTime.h"
#include "Components\CutSceneWaypoint.h"
#include "Components\Transform.h"
#include "CameraManager.h"
#include "Components\Camera.h"
#include "Engine.h"
#include "DirectXMath.h"
#include "EventSystem.h"
#include "ModuleInterface.h"
#include "InputManager.h"
#include "UIElement.h"
#include "UIComponents\UIImage.h"
#include "UIComponents\UICanvas.h"
#include "AI\AIModule.h"

CutScene::CutScene(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pGameCamera = nullptr;
	m_pCutSceneCamera = nullptr;
	m_pTangentWaypoint = nullptr;
	m_pStartWaypoint = nullptr;

	//m_pMoveFromWaypoint = nullptr;
	m_pMoveToWaypoint = nullptr;

	m_pAIModule = nullptr;

	m_bIsLooping = false;
	m_bIsRunning = false;
	m_bIsFinished = false;
	m_bFuncCalled = false;
	m_bIsSkipable = true;

	m_pSkipKeyboardImage = new Texture();
	m_pSkipKeyboardImage->SetImage(Inferno::GetImage(L"UI_SkipKeyboard.dds"));

	m_pSkipControllerImage = new Texture();
	m_pSkipControllerImage->SetImage(Inferno::GetImage(L"UI_SkipController.dds"));
}

CutScene::~CutScene()
{
	delete m_pSkipKeyboardImage;
	delete m_pSkipControllerImage;
}

CutScene& CutScene::operator=(const CutScene& _rhs)
{
	if (this != &_rhs)
	{
		m_pGameCamera = _rhs.m_pGameCamera;
		m_pCutSceneCamera = _rhs.m_pCutSceneCamera;

		m_pStartWaypoint = _rhs.m_pStartWaypoint;

		//m_pMoveFromWaypoint = _rhs.m_pMoveFromWaypoint;
		m_pMoveToWaypoint = _rhs.m_pMoveToWaypoint;

		m_itTravelTimer = _rhs.m_itTravelTimer;

		m_bIsFinished = _rhs.m_bIsFinished;
		m_bFuncCalled = _rhs.m_bFuncCalled;
		m_bIsLooping = _rhs.m_bIsLooping;
		m_bIsRunning = _rhs.m_bIsRunning;

		SAFE_DELETE(m_pSkipKeyboardImage);
		m_pSkipKeyboardImage = new Texture();
		m_pSkipKeyboardImage->SetImage(_rhs.m_pSkipKeyboardImage->GetImage());
		m_pSkipKeyboardImage->SetOffsets(_rhs.m_pSkipKeyboardImage->GetOffsets());
		m_pSkipKeyboardImage->m_Name = _rhs.m_pSkipKeyboardImage->m_Name;

		SAFE_DELETE(m_pSkipControllerImage);
		m_pSkipControllerImage = new Texture();
		m_pSkipControllerImage->SetImage(_rhs.m_pSkipControllerImage->GetImage());
		m_pSkipControllerImage->SetOffsets(_rhs.m_pSkipControllerImage->GetOffsets());
		m_pSkipControllerImage->m_Name = _rhs.m_pSkipControllerImage->m_Name;
	}

	return *this;
}

IComponent& CutScene::operator=(IComponent& _rhs)
{
	CutScene* temp = (CutScene*)&_rhs;

	if (temp == nullptr)
		return *this;

	return operator=(*temp);
}

IComponent* CutScene::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	CutScene* temp = new CutScene(_gameObject, _transform);

	*temp = *this;

	return temp;
}

void CutScene::Init()
{
	// Resets any internal variables since we don't truly delete until the program exits
	m_pGameCamera = nullptr;
	m_pCutSceneCamera = nullptr;
	m_pStartWaypoint = nullptr;
	m_pMoveToWaypoint = nullptr;
	m_pTangentWaypoint = nullptr;
	m_pAIModule = nullptr;
	m_fOnCutSceneEndFunc = nullptr;

	m_bIsLooping = false;
	m_bIsRunning = false;
	m_bIsFinished = false;
	m_bFuncCalled = false;
	m_bIsSkipable = true;
}

void CutScene::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{
	ObjectData* data = _dataMap.find(GetInstanceID())->second;

	Property* prop = data->GetProperty(std::string("StartingWaypoint"));
	int id = atoi(prop->value.c_str());
	if (id > 0)
	{
		CutSceneWaypoint* waypoint = (CutSceneWaypoint*)(_objectMap.find((unsigned int)id)->second);
		SetStartWaypoint(waypoint);
	}

	prop = data->GetProperty(std::string("CutSceneCamera"));
	id = atoi(prop->value.c_str());
	if (id > 0)
	{
		Camera* cam = (Camera*)(_objectMap.find((unsigned int)id)->second);
		SetCutSceneCamera(cam);
	}

	prop = data->GetProperty(std::string("Looping"));
	id = atoi(prop->value.c_str());
	bool looping = Inferno::to_bool(id);
	SetIsLooping(looping);
}

void CutScene::Update()
{
	// Don't update if this cutscene is not running.
	if (m_bIsRunning == false)
		return;

	if (m_bIsFinished == false)
		UpdateCameraLocation();
	else
		EndCutScene();
}

void CutScene::OnTriggerEnter(Collider* _col)
{
	StartCutScene();
}

void CutScene::OnEnable()
{
	using namespace Inferno_UI;

	// Create the Skip Cutscene Canvas.
	UICanvas* cutSceneCanvas = nullptr;
	unsigned int cutSceneCanvasSlot;

	UIRenderer* skipImage = nullptr;

	cutSceneCanvas = GetCanvasByName("CutScene");
	if (cutSceneCanvas == nullptr)
	{
		CreateNewCanvas("CutScene", &cutSceneCanvas, &cutSceneCanvasSlot);
	}

	// Create the skip with keyboard UI Element.
	m_pSkipKeyboard = new UIElement();
	m_pSkipKeyboard->SetName("SkipKeyboard");
	m_pSkipKeyboard->SetActive(false);

	// Set Messege Anchors, Position and Bounds.
	RectTransform* transform;
	transform = m_pSkipKeyboard->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMRIGHT);
	transform->SetBounds(Vector2(224.0f, 128.0f));
	transform->SetPosition(-112.0f, -64.0f);

	// Add the Renderer component and set the texture for this element
	skipImage = m_pSkipKeyboard->AddComponent<UIRenderer>();
	skipImage->SetTexture(m_pSkipKeyboardImage);
	skipImage->SetLayer(0);

	// Create the skip with keyboard UI Element.
	m_pSkipController = new UIElement();
	m_pSkipController->SetName("SkipController");
	m_pSkipController->SetActive(false);

	// Set Messege Anchors, Position and Bounds.
	transform = m_pSkipController->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMRIGHT);
	transform->SetBounds(Vector2(224.0f, 128.0f));
	transform->SetPosition(-112.0f, -64.0f);

	// Add the Renderer component and set the texture for this element
	skipImage = m_pSkipController->AddComponent<UIRenderer>();
	skipImage->SetTexture(m_pSkipControllerImage);
	skipImage->SetLayer(0);

	// Add both elements to the canvas
	AddToView(m_pSkipKeyboard, "CutScene");
	AddToView(m_pSkipController, "CutScene");

	// Hold on to the current game scene camera.
	m_pGameCamera = CameraManager::Main();

	IComponent::OnEnable();
}

void CutScene::OnDisable()
{
	m_itSkipTimer.SetTime(0.0f);
	m_itSkipTimer.Reset();
	m_itTravelTimer.SetTime(0.0f);
	m_itTravelTimer.Reset();

	IComponent::OnDisable();
}

void CutScene::PlayCutScene()
{
	m_bIsRunning = true;
}

void CutScene::PauseCutScene()
{
	m_bIsRunning = false;
}

void CutScene::SkipCutScene(ACTION_STATUS _status)
{
	if (_status != ACTION_STATUS::AS_PRESSED)
	{
		return;
	}

	EndCutScene();
}

void CutScene::StartCutScene()
{
	// If our MoveFrom and MoveTo Waypoints have been set.
	if (m_pStartWaypoint != nullptr)
	{
		m_pMoveToWaypoint = m_pStartWaypoint->GetNextWaypoint();

		if (m_pMoveToWaypoint != nullptr)
		{
			// Set the time it will take to reach our next waypoint.
			float travelTime = m_pMoveToWaypoint->GetWaypointTime();
			m_itTravelTimer.SetTime(travelTime);
			m_itTravelTimer.SetOnTimerEndFunction(std::bind(&CutScene::UpdateWaypoints, this));

			// We're moving from our start and moving to our starts next.
			Vector3 garbage;
			m_pStartWaypoint->transform->WorldMatrix().Decompose(m_sMoveFromWaypoint.Position, m_sMoveFromWaypoint.Rotation, garbage);
			m_pMoveToWaypoint->transform->WorldMatrix().Decompose(m_sMoveToWaypoint.Position, m_sMoveToWaypoint.Rotation, garbage);
			m_pTangentWaypoint = m_pStartWaypoint->GetTangentWaypoint();
		}
	}

	EventSystem* pEventSystem = EventSystem::GetInstance();
	// Stop the player from recieving input.
	bool playerReceiveInput = false;
	pEventSystem->SendEventNow("PlayerRecieveInput", &playerReceiveInput);

	// Enable minion spawners.
	bool active = true;
	pEventSystem->SendEventNow("CutSceneActive", &active);

	pEventSystem->SendEventNow("CloseTowerMenu", nullptr);

	// Disable the player HUD
	UICanvas* hudCanvas = Inferno_UI::GetCanvasByName("HUD");
	if (hudCanvas != nullptr)
		hudCanvas->GetUIElement()->SetActive(false);

	// Disable the extended HUD
	//hudCanvas = Inferno_UI::GetCanvasByName("ExtendedHUD");
	//if (hudCanvas != nullptr)
	//	hudCanvas->GetUIElement()->SetActive(false);

	// Set the time required to pass before the cutscene can be skipped
	m_itSkipTimer.SetTime(3.0f);
	m_itSkipTimer.SetOnTimerEndFunction(std::bind(&CutScene::SetCutSceneSkippable, this));

	// If this cutscenes camera has been set, set the main game camera to the cutscene camera
	if (m_pCutSceneCamera != nullptr)
		CameraManager::SetMainCamera(m_pCutSceneCamera);

	// Disable Behavior Trees for AI Entities.
	InfernoEngine_AI::EnterCutScene();

	// Let the cutscene play.
	PlayCutScene();
}

void CutScene::EndCutScene()
{
	m_bIsRunning = false;

	// Unregister from events.
	InfernoEngine_Input::UnregisterEventFromActionBinding("Interact", this, BindActionFunction(&CutScene::SkipCutScene, this));
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&CutScene::OnControllerChange, this));

	// Destroy the cutscene canvas.
	//UICanvas* cutSceneCanvas = nullptr;
	//cutSceneCanvas = Inferno_UI::GetCanvasByName("CutScene");
	//if (cutSceneCanvas != nullptr)
	//	Inferno_UI::DestroyCanvas("CutScene");

	m_pSkipController->SetActive(false);
	m_pSkipKeyboard->SetActive(false);

	// Enable the player HUD.
	UICanvas* hudCanvas = Inferno_UI::GetCanvasByName("HUD");
	if (hudCanvas != nullptr)
		hudCanvas->GetUIElement()->SetActive(true);

	// Enable the player HUD.
	//hudCanvas = Inferno_UI::GetCanvasByName("ExtendedHUD");
	//if (hudCanvas != nullptr)
	//	hudCanvas->GetUIElement()->SetActive(true);

	// Enable player input.
	bool playerReceiveInput = true;
	EventSystem::GetInstance()->SendEventNow("PlayerRecieveInput", &playerReceiveInput);

	// Enable minion spawners.
	bool active = false;
	EventSystem::GetInstance()->SendEventNow("CutSceneActive", &active);

	// Set the main camera back to the game camera.
	CameraManager::SetMainCamera(m_pGameCamera);

	// Re-Enable AI Enities
	InfernoEngine_AI::ExitCutScene();

	if (m_fOnCutSceneEndFunc)
		m_fOnCutSceneEndFunc();

	Inferno::Destroy(this->gameObject);
}

// Returns true when the cutscene is finished playing. false if it is not.
bool CutScene::IsFinished()
{
	return m_bIsFinished;
}

// Sets the looping bool
void CutScene::SetIsLooping(bool _isLooping)
{
	m_bIsLooping = _isLooping;
}

// Set the camera used by this cutscene.
void CutScene::SetCutSceneCamera(Camera* _camera)
{
	m_pCutSceneCamera = _camera;
}

// Sets the start waypoing of this cutscene.
void CutScene::SetStartWaypoint(CutSceneWaypoint* _waypoint)
{
	m_pStartWaypoint = _waypoint;
}

// Sets the function to be called when the cutscene ends.
void CutScene::SetOnCutSceneEndFunction(OnCutSceneEndFunc _func)
{
	m_fOnCutSceneEndFunc = _func;
}

// Sets if the cutscene can be skipped or not
void CutScene::SetIsSkipable(bool skipable)
{
	m_bIsSkipable = skipable;
}

// Returns the start waypoing of the cutscene
CutSceneWaypoint* CutScene::GetStartWaypoint() const
{
	return m_pStartWaypoint;
}

// Interpolates current and next waypoint transforms based on time ratio.
void CutScene::UpdateCameraLocation()
{
	using namespace DirectX;

	float tweenTime = m_itTravelTimer.TimeRemaining();
	float travelTime = m_itTravelTimer.GetSetTimeSeconds();

	if (travelTime > 0.0f == false)
	{
		travelTime = tweenTime = 1.0f;
	}

	float lerpRatio = tweenTime / travelTime;

	Vector3 MoveFromPos = m_sMoveFromWaypoint.Position;
	Quaternion moveFromQuat = m_sMoveFromWaypoint.Rotation;

	Vector3 MoveToPos = m_sMoveToWaypoint.Position;
	Quaternion moveToQuat = m_sMoveToWaypoint.Rotation;

	Vector3 posResult;

	if (m_pTangentWaypoint)
	{
		Vector3 tangentPoint = m_pTangentWaypoint->transform->GetPosition();
		posResult = Vector3::Slerp(MoveToPos, MoveFromPos, tangentPoint, lerpRatio);
	}
	else
		XMStoreFloat3(&posResult, XMVectorLerp(XMLoadFloat3(&MoveToPos), XMLoadFloat3(&MoveFromPos), lerpRatio));

	Quaternion quatResult;
	XMStoreFloat4(&quatResult, XMQuaternionSlerp(XMLoadFloat4(&moveToQuat), XMLoadFloat4(&moveFromQuat), lerpRatio));

	Vector3 scaleResult = Vector3(1.0f, 1.0f, 1.0f);

	Matrix4 matResult = Matrix4::Affine(posResult, quatResult, scaleResult);

	m_pCutSceneCamera->transform->SetLocalMatrix(matResult);
}

void CutScene::UpdateWaypoints()
{
	// If there is no camera we should quit out
	if (m_pCutSceneCamera == nullptr)
	{
		EndCutScene();
		return;
	}

	CutSceneWaypoint* nextWaypoint = m_pMoveToWaypoint->GetNextWaypoint();
	m_pTangentWaypoint = m_pMoveToWaypoint->GetTangentWaypoint();

	// If the waypoint we were traveling to has a valid next waypoint..
	if (nextWaypoint != nullptr)
	{
		// The waypoint we were traveling to is now the waypoint we are traveling from.
		Vector3 garbage;
		m_pCutSceneCamera->transform->WorldMatrix().Decompose(m_sMoveFromWaypoint.Position, m_sMoveFromWaypoint.Rotation, garbage);
		//m_sMoveFromWaypoint.Position = m_sMoveToWaypoint.Position;
		//m_sMoveFromWaypoint.Rotation = m_sMoveToWaypoint.Rotation;

		// We're now traveling to our next available waypoint.
		m_pMoveToWaypoint = nextWaypoint;
		m_pMoveToWaypoint->transform->WorldMatrix().Decompose(m_sMoveToWaypoint.Position, m_sMoveToWaypoint.Rotation, garbage);

		// Set the time it will take to reach our next waypoint.
		float travelTime = m_pMoveToWaypoint->GetWaypointTime();
		m_itTravelTimer.SetTime(travelTime);
		m_itTravelTimer.SetOnTimerEndFunction(std::bind(&CutScene::UpdateWaypoints, this));
	}
	else
	{
		// If we're looping..
		if (m_bIsLooping == true)
		{
			// The waypoint we're traveling to is now the waypoint we're traveling from.
			Vector3 garbage;
			m_pCutSceneCamera->transform->WorldMatrix().Decompose(m_sMoveFromWaypoint.Position, m_sMoveFromWaypoint.Rotation, garbage);

			// The waypoint we're traveling to is now our starting waypoint.
			m_pMoveToWaypoint = m_pStartWaypoint;
			m_pMoveToWaypoint->transform->WorldMatrix().Decompose(m_sMoveToWaypoint.Position, m_sMoveToWaypoint.Rotation, garbage);

			// Set the time it will take to reach our next waypoint.
			float travelTime = m_pMoveToWaypoint->GetWaypointTime();
			m_itTravelTimer.SetTime(travelTime);
			m_itTravelTimer.SetOnTimerEndFunction(std::bind(&CutScene::UpdateWaypoints, this));
		}
		else // The cutscene has ended.
		{
			m_bIsFinished = true;
		}
	}
}

// Called at the end of this cutscenes skip timer.
void CutScene::SetCutSceneSkippable()
{
	if (!m_bIsSkipable)
		return;

	// Check to see which control device our player is using and set the skip messege accordingly.
	bool usingController = InputManager::UsingController();
	m_pSkipKeyboard->SetActive(!usingController);
	m_pSkipController->SetActive(usingController);

	// Register the interact button to skip the cutscene.
	InfernoEngine_Input::RegisterEventToActionBinding("Interact", this, BindActionFunction(&CutScene::SkipCutScene, this));

	// Register for a control change event.
	InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&CutScene::OnControllerChange, this));
}

// Called when keyboard switches to controller or vice-versa
void CutScene::OnControllerChange(bool _usingController)
{
	m_pSkipKeyboard->SetActive(!_usingController);
	m_pSkipController->SetActive(_usingController);
}