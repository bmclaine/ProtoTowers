#include "UIManager.h"

#include "../../Interface/Public/ModuleInterface.h"
#include "EventSystem.h"
#include "UIComponents\RectTransform.h"
#include "UIComponents\UICanvas.h"
#include "UIComponents\UIEventSystem.h"
#include "UIController.h"
#include "UIElement.h"

#include <Windows.h>

// ===== Constructor / Destructor ===== //
UIManager::UIManager() : Registry<UIEventSystem>(16), m_fcFlags(UM_MAX_FLAGS)
{
	m_bIgnoreMouseInput = false;
	m_pUIController = nullptr;
	m_pFocusedSystem = nullptr;
	m_pUIController = new UIController();
}

UIManager::~UIManager()
{
	if (m_pUIController != nullptr)
		delete m_pUIController;

	using namespace InfernoEngine_Input;
	UnregisterEventFromMouseMovement(this, BindMouseMovementFunction(&UIManager::InputEvent_MouseMovement, this));
	UnregisterEventFromActionBinding("Mouse", this, BindActionFunction(&UIManager::InputEvent_MouseButton, this));

	EventSystem::GetInstance()->UnregisterFromEvent("ResolutionChange", this, BindEventFunction(&UIManager::OnResolutionChange, this));

	size_t count = m_vPendingDestroyList.size();
	for (size_t i = 0; i < count; ++i) {
		delete m_vPendingDestroyList[i];
	}
	m_vPendingDestroyList.clear();

	count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vCanvasList[i] != nullptr)
			delete m_vCanvasList[i];
	}
	m_vCanvasList.clear();
	m_vCanvasNames.clear();
}
// ==================================== //

// ===== Interface ===== //
void UIManager::Init()
{
	size_t count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		m_vCanvasList[i]->Init();
	}

	if (m_pUIController != nullptr) {
		m_pUIController->Init();
	}
}

void UIManager::PostInit()
{
	using namespace InfernoEngine_Input;
	using namespace InfernoEngine_Graphics;

	m_iScaleBufferID = GetConstantBuffer(L"ScaleFactor");

	RegisterEventToMouseMovement(this, BindMouseMovementFunction(&UIManager::InputEvent_MouseMovement, this));
	RegisterEventToActionBinding("Mouse", this, BindActionFunction(&UIManager::InputEvent_MouseButton, this));

	EventSystem::GetInstance()->RegisterForEvent("ResolutionChange", this, BindEventFunction(&UIManager::OnResolutionChange, this));

	size_t count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		m_vCanvasList[i]->PostInit();
	}

	if (m_pUIController != nullptr) {
		m_pUIController->PostInit();
	}

	m_fcFlags.SetFlag((unsigned int)UM_POSTINITCALLED_FLAG, (unsigned int)true);
}

void UIManager::UnregisterEventSystem(UIEventSystem* _eventSystem)
{
	if (_eventSystem == m_pFocusedSystem) {
		m_pFocusedSystem = nullptr;
	}

	UnregisterObject(_eventSystem);
}

// === RegisterForCanvasScaleChange
//  IN : string& - Name of the Canvas to register with
//       void* - The owning object of the function to be called.
//       CanvasScaleChangeFunc - Func to be registered. Format of void(void)
//  OUT : void
//
//  Registers a Function to be called when a specific canvas changes it's scale size. 
void UIManager::RegisterForCanvasScaleChange(std::string& _canvasName, void* _callingObject, std::function<void(void)> _func)
{
	size_t i = 0;
	size_t count = m_vCanvasNames.size();
	for (; i < count; ++i) {
		if (m_vCanvasNames[i] == _canvasName) {
			break;
		}
	}

	if (i == count) {
		return;
	}

	m_vCanvasList[i]->GetComponent<UICanvas>()->RegisterForCanvasScaleChange(_callingObject, _func);
}

// === UnregisterFromCanvasScaleChange
	//  IN : string& - Name of the Canvas to unregister from
	//       void* - The owning object of the function that was registered.
	//  OUT : void
	// 
	//  Unregisters a CanvasScaleChange Function from a specific canvas.
void UIManager::UnregisterFromCanvasScaleChange(std::string& _canvasName, void* _callingObject)
{
	size_t i = 0;
	size_t count = m_vCanvasNames.size();
	for (; i < count; ++i) {
		if (m_vCanvasNames[i] == _canvasName) {
			break;
		}
	}

	if (i == count) {
		return;
	}

	m_vCanvasList[i]->GetComponent<UICanvas>()->UnregisterFromCanvasScaleChange(_callingObject);
}

// === Update
//  IN : void
//  Out : void
//
//  Checks for input interactions with any registered UIEventSystems, passing the type of event that was trigger to the affected
//  UIEventSystem. Checks for Mouse Input by default, and then calls update on its UIController, which can handle advanced input.
void UIManager::Update()
{
	// === Can we delete the UI this Frame?
	if (m_fcFlags.GetFlag((unsigned int)UM_DESTROYPENDINGUI_NEXTFRAME_FLAG)) {
		m_fcFlags.SetFlag((unsigned int)UM_DESTROYPENDINGUI_NEXTFRAME_FLAG, (unsigned int)false);
	}
	else {
		for (size_t i = 0; i < m_vPendingDestroyList.size(); ++i) {
			delete m_vPendingDestroyList[i];
		}
		m_vPendingDestroyList.clear();
	}

	// === Update the UIController
	if (m_pUIController != nullptr) {
		m_pUIController->Update();
	}

	// === Update all Canvas'
	size_t count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vCanvasList[i] != nullptr && m_vCanvasList[i]->IsActiveInHierarchy()) {
			m_vCanvasList[i]->Update();
		}
	}
}

// === CreateNewCanvas
//  IN : UICanvas* - { Out Parameter } Pointer to the created canvas
//  OUT : unsigned int - The slot that the canvas ws created in
//
//  Creates a new Canvas, at the first available slot, and returns that slot ID. Returns the created Canvas through parameters.
unsigned int UIManager::CreateNewCanvas(std::string _canvasName, UICanvas** _createdCanvas)
{
	UIElement* newCanvasElement = new UIElement();
	newCanvasElement->SetName(_canvasName);
	*_createdCanvas = newCanvasElement->AddComponent<UICanvas>();
	(*_createdCanvas)->SetTargetResolution(m_fTargetResolutionWidth, m_fTargetResolutionHeight);
	newCanvasElement->GetTransform()->Lock(false);
	newCanvasElement->GetTransform()->SetCanvasScale(Vector2(m_fWidthScale, m_fHeightScale));
	newCanvasElement->GetTransform()->Lock(true);
	
	if (!m_fcFlags.GetFlag(UM_POSTINITCALLED_FLAG)) {
		newCanvasElement->Init();
		newCanvasElement->PostInit();
		newCanvasElement->OnEnable();
	}

	size_t count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vCanvasList[i] == nullptr) {
			m_vCanvasList[i] = newCanvasElement;
			return (unsigned int)i;
		}
	}

	m_vCanvasList.push_back(newCanvasElement);
	m_vCanvasNames.push_back(_canvasName);
	return (unsigned int)m_vCanvasList.size() - 1;
}

// === DestroyCanvas
//  IN : string - Name of the Canvas
//  OUT : void
//
//  Destroys a Canvas and all UIElements that are attached to the Canvas.
void UIManager::DestroyCanvas(std::string _canvasName)
{
	size_t i = 0;
	size_t count = m_vCanvasNames.size();
	for (; i < count; ++i) {
		if (m_vCanvasNames[i] == _canvasName) {
			break;
		}
	}

	if (i == count) {
		return;
	}

	UIElement* tempPtr = m_vCanvasList[i];
	m_vCanvasList[i] = m_vCanvasList[count - 1];
	m_vPendingDestroyList.push_back(tempPtr);
	m_vCanvasList.erase(--m_vCanvasList.end());

	tempPtr->OnDisable();

	std::string tempStr = m_vCanvasNames[i];
	m_vCanvasNames[i] = m_vCanvasNames[count - 1];
	m_vCanvasNames[count - 1] = tempStr;
	m_vCanvasNames[count - 1] = "";
	m_vCanvasNames.erase(--m_vCanvasNames.end());

	m_fcFlags.SetFlag((unsigned int)UM_DESTROYPENDINGUI_NEXTFRAME_FLAG, (unsigned int)true);
}

// === AddToView
//  IN : UIElement* - The Pointer to the UIElement to add
//       unsigned int - The slot of the canvas that the element should be added to
//  OUT : bool - Whether or not the element was added to the desired canvas
//
//  Adds the given UIElement to a desired UICanvas. Returns true if the canvas was found and the element was added, false otherwise.
bool UIManager::AddToView(UIElement* _element, unsigned int _canvasSlot)
{
	if (_canvasSlot >= m_vCanvasList.size() || m_vCanvasList[_canvasSlot] == nullptr) {
		return false;
	}

	_element->GetTransform()->SetParent(m_vCanvasList[_canvasSlot]->GetTransform());
	if (m_fcFlags.GetFlag((unsigned int)UM_POSTINITCALLED_FLAG)) {
		_element->PostInit();
		_element->OnEnable();
	}
	
	return true;
}

// === AddToView
//  IN : UIElement* - The Pointer to the UIElement to add
//       std::string - The name of the Canvas to add the element to
//  OUT : bool - Whether or not the element was added to the desired canvas
//
//  Adds the given UIElement to a desired UICanvas. Returns true if the canvas was found and the element was added, false otherwise.
bool UIManager::AddToView(UIElement* _element, std::string _canvasName)
{
	size_t count = m_vCanvasNames.size();
	size_t i = 0;
	for (; i < count; ++i) {
		if (m_vCanvasNames[i] == _canvasName) {
			break;
		}
	}

	if (i == count) {
		return false;
	}

	_element->GetTransform()->SetParent(m_vCanvasList[i]->GetTransform());
	if (m_fcFlags.GetFlag((unsigned int)UM_POSTINITCALLED_FLAG)) {
		_element->PostInit();
		_element->OnEnable();
	}
	
	return true;
}

void UIManager::ClearView()
{
	m_pUIController->SetCurrentSelectable(nullptr);

	size_t count = m_vCanvasNames.size();
	while (m_vCanvasNames.size() > 1) {
		DestroyCanvas(m_vCanvasNames[1]);
	}
}
// ===================== //

// === Input Events === //
void UIManager::InputEvent_MouseMovement(POINT _cursorPosition, POINT _displacement)
{
	Vector2 cursorPos = Vector2((float)_cursorPosition.x, (float)_cursorPosition.y);

	if (m_pFocusedSystem != nullptr && m_pFocusedSystem->IsLockedForControls()) {
		m_pFocusedSystem->InvokeMouseHover(_cursorPosition, _displacement);
		return;
	}

	for (size_t i = 0; i < m_vRegisteredObjects.size(); ++i) {
		if (m_vRegisteredObjects[i]->IsActiveInHierachry()) {
			if (m_vRegisteredObjects[i]->GetTransform()->ContainsPoint(cursorPos)) {
				SetFocusedSystem(m_vRegisteredObjects[i]);
				m_pFocusedSystem->InvokeMouseHover(_cursorPosition, _displacement);
				return;
			}
		}
	}

	SetFocusedSystem(nullptr);
}

void UIManager::InputEvent_MouseButton(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_DOWN)
		return;

	if (m_pFocusedSystem == nullptr)
		return;

	if (!m_pFocusedSystem->IsEnabled()) {
		SetFocusedSystem(nullptr);
		return;
	}

	switch (_status) {
	case ACTION_STATUS::AS_PRESSED:
		m_pFocusedSystem->InvokeEvent(UI_EVENTS::UI_EVENT_ONCLICK);
		break;
	case ACTION_STATUS::AS_RELEASED:
		m_pFocusedSystem->InvokeEvent(UI_EVENTS::UI_EVENT_ONRELEASED);
		break;
	}
}
// ==================== //

// ===== Events ===== //
void UIManager::OnResolutionChange(void* _pData)
{
	float width = *(static_cast<float*>(_pData));
	float height = *((static_cast<float*>(_pData)+1));
	m_fWidthScale = width / m_fTargetResolutionWidth;
	m_fHeightScale = height / m_fTargetResolutionHeight;

	using namespace InfernoEngine_Graphics;

	struct ScaleData { float XScale, YScale, XDimension, YDimension; };

	ScaleData data = { m_fWidthScale, m_fHeightScale, width, height };

	UpdateSubResource(m_iScaleBufferID, &data, sizeof(ScaleData));

	size_t count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		m_vCanvasList[i]->GetTransform()->Lock(false);
		m_vCanvasList[i]->GetTransform()->SetCanvasScale(Vector2(m_fWidthScale, m_fHeightScale));
		m_vCanvasList[i]->GetTransform()->Lock(true);
	}
}
// ================== //

// ===== Private Interface ===== //
void UIManager::SetFocusedSystem(UIEventSystem* _system)
{
	if (m_pFocusedSystem == _system) {
		return;
	}

	if (m_pFocusedSystem != nullptr) {
		m_pFocusedSystem->InvokeEvent(UI_EVENTS::UI_EVENT_ONHOVEREND);
	}

	m_pFocusedSystem = _system;

	if (m_pFocusedSystem != nullptr) {
		m_pFocusedSystem->InvokeEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN);
	}
}
// ============================= //

// ===== Accessors ===== //

// === IsIgnoringMouseInput
//  IN : void
//  OUT : bool - Whether or not the Mouse Input is being ignored.
//
//  Returns whether or not the UIManager is currently ignoring mouse input when checking for interaction with UIEventSystems.
bool UIManager::IsIgnoringMouseInput() const
{
	return m_bIgnoreMouseInput;
}

// === IsFocusedElementLocked
//  IN : void
//  OUT : bool - Whether or not the current focused element is locked
//
//  Checks to see if the current focused element is locked or not.
bool UIManager::IsFocusedElementLocked() const
{
	if (m_pFocusedSystem == nullptr) {
		return false;
	}

	return m_pFocusedSystem->IsLockedForControls();
}

// === GetUIController
//  IN : void
//  OUT : UIController* - Pointer to the UIController that the UIManager is currently using
// 
//  Returns the UIController that the UIManager is currently using.
UIController* UIManager::GetUIController() const
{
	return m_pUIController;
}

// === GetCanvas
//  IN : unsigned int - The slot of the Canvas to retrieve
//  OUT : UICanvas* - The Canvas at the specific slot
//
//  Returns the Canvas at a given slot. If there is no canvas at that slot, it returns a nullptr.
UICanvas* UIManager::GetCanvas(unsigned int _canvasSlot) const
{
	if (_canvasSlot >= m_vCanvasList.size()) {
		return nullptr;
	}

	return m_vCanvasList[_canvasSlot]->GetComponent<UICanvas>();
}

// === GetCanvasByName
//  IN : string - The name of the Canvas
//  OUT : UICanvas* - The desired Canvas, if it exists
//
//  Returns the Canvas found under the given name, if it exists.
UICanvas* UIManager::GetCanvasByName(std::string _canvasName) const
{
	size_t count = m_vCanvasNames.size();
	size_t index = 0;
	for (; index < count; ++index) {
		if (m_vCanvasNames[index] == _canvasName) {
			break;
		}
	}

	if (index == count) {
		return nullptr;
	}
	else {
		return m_vCanvasList[index]->GetComponent<UICanvas>();
	}
}

// === GetUIElementByName
//  IN : string - Name of the Element to look for
//       string - (Optional) Name of the Canvas to look in
//  OUT : UIElement* - The found UIElement
//
//  Looks for a UIElement with the given name, on the specified canvas, if one is specified. Returns the found element, or nullptr if no element
//  was found with that name
UIElement* UIManager::GetUIElementByName(std::string& _elementName, std::string& _canvasName)
{
	if (_canvasName == std::string()) {
		// === No specified canvas, look through them all
		UIElement* foundChild = nullptr;
		size_t count = m_vCanvasList.size();
		for (size_t i = 0; i < count; ++i) {
			foundChild = m_vCanvasList[i]->GetTransform()->GetChildByName(_elementName, true);
			if (foundChild != nullptr) {
				return foundChild;
			}
		}

		return nullptr;
	}
	else {
		// === Get the specified Canvas
		UICanvas* desiredCanvas = GetCanvasByName(_canvasName);
		if (desiredCanvas == nullptr) {
			return nullptr;
		}

		return desiredCanvas->GetTransform()->GetChildByName(_elementName, true);
	}
}

// === GetCurrentSelectable
//  IN : void
//  OUT : UISelectable* - Pointer to the Selectable that is selected, nullptr if none
//
//  Returns the currently selected UISelectable
UISelectable* UIManager::GetCurrentSelectable()
{
	if (m_pUIController)
		return m_pUIController->GetCurrentSelectable();

	return nullptr;
}
// ===================== //

// ===== Mutators ===== //

// === IgnoreMouseInput
//  IN : bool - Whether or not the input should be ignored
//  OUT : void
//
//  Sets whether or not the UIManager will check mouse input for interactions with the registered UIEventSystems
void UIManager::IgnoreMouseInput(bool _value)
{
	m_bIgnoreMouseInput = _value;
}

// === SetUIController
//  IN : UIController - The desired controller for the UIManager to use
//  OUT : void
// 
//  Sets the UIController that the UIManager will use for handling advanced input / events. Handles cleaning up the old 
//  UIController, if there was one.
void UIManager::SetUIController(UIController* _controller)
{
	if (m_pUIController != nullptr)
		delete m_pUIController;

	m_pUIController = _controller;

	if (m_fcFlags.GetFlag((unsigned int)UM_POSTINITCALLED_FLAG) && m_pUIController != nullptr) {
		m_pUIController->Init();
		m_pUIController->PostInit();
	}
}

// === SetTargetResolution
//  IN : float - The target width
//       float - The target height
//  OUT : void 
//
//  Sets the Target Resolution that UI is designed for.
void UIManager::SetTargetResolution(float _width, float _height)
{
	m_fTargetResolutionWidth = _width;
	m_fTargetResolutionHeight = _height;

	size_t count = m_vCanvasList.size();
	for (size_t i = 0; i < count; ++i) {
		m_vCanvasList[i]->GetComponent<UICanvas>()->SetTargetResolution(_width, _height);
	}
}

// === SetCurrentSelectable
//  IN : UISelectable* - Pointer to the Selectable that should be focused on
//  OUT : void
//
//  Sets the UISelectable that the UIController should be focusing on
void UIManager::SetCurrentSelectable(UISelectable* _selectable)
{
	if (m_pUIController != nullptr) {
		m_pUIController->SetCurrentSelectable(_selectable);
	}
}
// ==================== //