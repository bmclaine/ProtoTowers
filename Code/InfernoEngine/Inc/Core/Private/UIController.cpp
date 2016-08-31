#include "UIController.h"

#include "../../Interface/Public/ModuleInterface.h"
#include "InfernoTime.h"
#include "UIComponents\UIButton.h"
#include "UIComponents\UIEventSystem.h"
#include "UIComponents\UISelectable.h"
#include "UIElement.h"

#define INPUT_COOLDOWN 0.2f

// ===== Constructor / Destructor ===== //
UIController::UIController()
{
	m_sBindingUp = "Forward";
	m_sBindingRight = "Right";
	m_sBindingInteract = "Interact";
	m_pCurrentSelectable = nullptr;
}

UIController::~UIController()
{
	UnregisterEvents(); 
}
// ==================================== //

// ===== Interface ===== //
void UIController::Init()
{

}

void UIController::PostInit()
{
	RegisterEvents();
}

void UIController::Update()
{
	m_fCooldown -= Time::TrueDeltaTime;
}

// === UnregisterEvents
//  IN : void
//  OUT : void
//
//  Unregisters all Events from the Input Manager.
void UIController::UnregisterEvents()
{
	using namespace InfernoEngine_Input;
	
	if (m_sBindingUp.size() != 0)
		UnregisterEventFromAxisBinding(m_sBindingUp.c_str(), this, BindAxisFunction(&UIController::AxisEvent_OnNavigateUp, this));
	if (m_sBindingRight.size() != 0)
		UnregisterEventFromAxisBinding(m_sBindingRight.c_str(), this, BindAxisFunction(&UIController::AxisEvent_OnNavigateRight, this));
	if (m_sBindingInteract.size() != 0)
		UnregisterEventFromActionBinding(m_sBindingInteract.c_str(), this, BindActionFunction(&UIController::Event_OnSelect, this));
}

// === RegisterEvents
//  IN : void
//  OUT : void
//
//  Registers all needed events with Input Manager. Gets called in the PostInit, but can be called later, if and only if the 
//  events were previously unregistered.
void UIController::RegisterEvents()
{
	using namespace InfernoEngine_Input;

	if (m_sBindingUp.size() != 0)
		RegisterEventToAxisBinding(m_sBindingUp.c_str(), this, BindAxisFunction(&UIController::AxisEvent_OnNavigateUp, this));
	if (m_sBindingRight.size() != 0)
		RegisterEventToAxisBinding(m_sBindingRight.c_str(), this, BindAxisFunction(&UIController::AxisEvent_OnNavigateRight, this));
	if (m_sBindingInteract.size() != 0)
		RegisterEventToActionBinding(m_sBindingInteract.c_str(), this, BindActionFunction(&UIController::Event_OnSelect, this));
}
// ===================== //

// ===== Event Functions ===== //
void UIController::AxisEvent_OnNavigateUp(float _axisValue)
{
	if (_axisValue > 0) {
		Event_OnNavigateUp(ACTION_STATUS::AS_PRESSED);
	}
	else if (_axisValue < 0) {
		Event_OnNavigateDown(ACTION_STATUS::AS_PRESSED);
	}
}

void UIController::AxisEvent_OnNavigateRight(float _axisValue)
{
	if (_axisValue > 0) {
		Event_OnNavigateRight(ACTION_STATUS::AS_PRESSED);
	}
	else if (_axisValue < 0) {
		Event_OnNavigateLeft(ACTION_STATUS::AS_PRESSED);
	}
}

void UIController::Event_OnNavigateUp(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_pCurrentSelectable == nullptr || !m_pCurrentSelectable->IsActiveInHierachry() || m_fCooldown > 0.0f)
			return;

		m_fCooldown = INPUT_COOLDOWN;

		UISelectable* nextSelectable = m_pCurrentSelectable->GetUpSelectable();
		if (nextSelectable != nullptr) {
			m_pCurrentSelectable->OnDeselect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONDESELECT);
			m_pCurrentSelectable = nextSelectable;
			m_pCurrentSelectable->OnSelect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONSELECT);
		}
	}
}

void UIController::Event_OnNavigateDown(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_pCurrentSelectable == nullptr || !m_pCurrentSelectable->IsActiveInHierachry() || m_fCooldown > 0.0f)
			return;

		m_fCooldown = INPUT_COOLDOWN;

		UISelectable* nextSelectable = m_pCurrentSelectable->GetDownSelectable();
		if (nextSelectable != nullptr && m_pCurrentSelectable->IsActiveInHierachry()) {
			m_pCurrentSelectable->OnDeselect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONDESELECT);
			m_pCurrentSelectable = nextSelectable;
			m_pCurrentSelectable->OnSelect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONSELECT);
		}
	}
}

void UIController::Event_OnNavigateRight(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_pCurrentSelectable == nullptr || !m_pCurrentSelectable->IsActiveInHierachry() || m_fCooldown > 0.0f)
			return;

		m_fCooldown = INPUT_COOLDOWN;

		UISelectable* nextSelectable = m_pCurrentSelectable->GetRightSelectable();
		if (nextSelectable != nullptr && m_pCurrentSelectable->IsActiveInHierachry()) {
			m_pCurrentSelectable->OnDeselect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONDESELECT);
			m_pCurrentSelectable = nextSelectable;
			m_pCurrentSelectable->OnSelect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONSELECT);
		}
	}
}

void UIController::Event_OnNavigateLeft(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_pCurrentSelectable == nullptr || !m_pCurrentSelectable->IsActiveInHierachry() || m_fCooldown > 0.0f)
			return;

		m_fCooldown = INPUT_COOLDOWN;

		UISelectable* nextSelectable = m_pCurrentSelectable->GetLeftSelectable();
		if (nextSelectable != nullptr && nextSelectable->IsActiveInHierachry()) {
			m_pCurrentSelectable->OnDeselect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONDESELECT);
			m_pCurrentSelectable = nextSelectable;
			m_pCurrentSelectable->OnSelect();
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONSELECT);
		}
	}
}

void UIController::Event_OnSelect(ACTION_STATUS _status)
{
	if (_status == ACTION_STATUS::AS_PRESSED) {
		if (m_pCurrentSelectable != nullptr && m_pCurrentSelectable->IsActiveInHierachry()) {
			m_pCurrentSelectable->GetUIElement()->GetEventSystem()->InvokeEvent(UI_EVENTS::UI_EVENT_ONCLICK);
		}
	}
}
// =========================== //

// ===== Accessors ===== //
UISelectable* UIController::GetCurrentSelectable() const
{
	return m_pCurrentSelectable;
}
// ====================== //

// ===== Mutators ===== //
void UIController::SetCurrentSelectable(UISelectable* _selectable)
{
	if (m_pCurrentSelectable != nullptr) {
		m_pCurrentSelectable->OnDeselect();
	}

	m_pCurrentSelectable = _selectable;

	if (m_pCurrentSelectable != nullptr) {
		m_pCurrentSelectable->OnSelect();
	}
}
// ==================== //