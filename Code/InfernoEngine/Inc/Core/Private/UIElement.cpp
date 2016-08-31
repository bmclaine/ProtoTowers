#include "UIElement.h"

#include "UIComponents\UIEventSystem.h"

// ===== Constructor / Destructor ===== //
UIElement::UIElement()
{
	m_vComponents.reserve(12);

	m_pRectTransform = new RectTransform(this);
	m_pEventSystem = nullptr;
	m_sCanvasName = "";
	m_fcFlags.SetFlag((unsigned int)UE_ACTIVE_FLAG, (unsigned int)true);
}

UIElement::~UIElement()
{
	OnDestroy();

	if (m_pRectTransform != nullptr)
		delete m_pRectTransform;

	if (m_pEventSystem != nullptr)
		delete m_pEventSystem;

	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		delete m_vComponents[i];
	}
	m_vComponents.clear();
}
// ==================================== //

// ===== Interface ===== //

// === Init
//  IN : void
//  OUT : void
//
//  Handles Initialization of the UIElement and calls Init() on the UIComponents that are attached to this UIElement.
void UIElement::Init()
{
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->Init();
	}
}

// === PostInit
//  IN : void
//  OUT : void
// 
//  Handles Post-Initialization of the UIElement and calls PostInit() on the UIComponents that are attached to this UIElement.
void UIElement::PostInit()
{
	m_fcFlags.SetFlag((unsigned int)UE_POSTINIT_CALLED_FLAG, (unsigned int)true);

	if (m_pEventSystem != nullptr) {
		m_pEventSystem->PostInit();
	}

	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->PostInit();
	}

	if (m_pRectTransform != nullptr) {
		m_pRectTransform->PostInit();
	}
}

// === Update
//  IN : void
//  OUT : void
//
//  Handles Updating the UIComponents that are attached to this UIElement. Calls Update on all the components first, then calls
//  LateUpdate on all the components.
void UIElement::Update()
{
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vComponents[i]->IsActiveInHierachry())
			m_vComponents[i]->Update();
	}

	count = m_pRectTransform->m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_pRectTransform->m_vChildren[i]->m_pUIElement->IsActiveInHierarchy())
			m_pRectTransform->m_vChildren[i]->m_pUIElement->Update();
	}
}

// === LateUpdate
//  IN : void
//  OUT : void
//
// Handles the LateUpdate on all UIComponents and children attached to this UIElement
void UIElement::LateUpdate()
{
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->LateUpdate();
	}

	count = m_pRectTransform->m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_pRectTransform->m_vChildren[i]->m_pUIElement->LateUpdate();
	}
}

// === CompareTag
//  IN - string - string to compare against this UIElement's tag.
//  OUT : bool - Whether or not the two tags match.
//
//  Checks to see if the specified tag matches the tag of this UIElement.
bool UIElement::CompareTag(std::string _tag) const
{
	return (m_sTag.compare(_tag) == 0 ? true : false);
}

void UIElement::OnEnable()
{
	// === Enable all Components
	for (size_t i = 0; i < m_vComponents.size(); ++i) {
		m_vComponents[i]->OnEnable();
	}

	// === Enable all Children
	m_pRectTransform->OnEnable();
}

void UIElement::OnDisable()
{
	// === Disable all Components
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->OnDisable();
	}

	m_pRectTransform->OnDisable();
}

void UIElement::OnDestroy()
{
	// === Call OnDestroy on all Components
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->OnDestroy();
	}
}
// ===================== //

// === Accessors === //

// === IsActive
//  IN : void
//  OUT : bool - Whether or not this UIElement is active.
//
//  Returns whether or not this UIElement is currently active.
bool UIElement::IsActive() const
{
	return m_fcFlags.GetFlag(UE_ACTIVE_FLAG);
}

// === IsActiveInHierarchy
//  IN : void
//  OUT : bool - Whether or not this UIElement is currently active.
//
//  Returns whether or not this UIElement is currently active, taking into account for whether or no it's parent is active.
bool UIElement::IsActiveInHierarchy() const
{
	if (m_pRectTransform->m_pParent == nullptr) {
		return m_fcFlags.GetFlag(UE_ACTIVE_FLAG);
	}

	return m_fcFlags.GetFlag(UE_ACTIVE_FLAG) && m_pRectTransform->m_pParent->GetUIElement()->IsActiveInHierarchy();
}

// GetCanvasName
//  IN : void
//  OUT : string - Canvas Name
//
//  Returns the Name of the Canvas that this UIElement is attached to. If it's not on any canvas, returns nullptr;
std::string UIElement::GetCanvasName() const
{
	return m_sCanvasName;
}

// === GetName
//  IN : void
//  OUT : string - Name of the UIElement.
// 
//  Returns the name of the UIElement.
std::string UIElement::GetName() const
{
	return m_sName;
}

// === GetTag
//  IN : void
//  OUT : string - Tag of the UIElement.
//
//  Returns the tag of the UIElement.
std::string UIElement::GetTag() const
{
	return m_sTag;
}

// === GetTransform
//  IN : void
//  OUT : RectTransform* - Pointer to the RectTransform of this UIElement.
//
// Returns the RectTransform that is attached to this UIElement.
RectTransform* UIElement::GetTransform()
{
	return m_pRectTransform;
}

UIEventSystem* UIElement::GetEventSystem()
{
	if (m_pEventSystem == nullptr) {
		m_pEventSystem = new UIEventSystem(this);
		if (m_fcFlags.GetFlag((unsigned int)UE_POSTINIT_CALLED_FLAG)) {
			m_pEventSystem->PostInit();
		}
	}

	return m_pEventSystem;
}
// ================= //

// === Mutators === //

// === SetActive
//  IN : bool - Whether or not the UIElement should be active.
//  OUT : void
//
//  Sets whether or not the UIElement should be active.
void UIElement::SetActive(bool _value)
{
	if (m_fcFlags.GetFlag((unsigned int)UE_ACTIVE_FLAG) != _value) {
		if (_value) {
			OnEnable();
		}
		else {
			OnDisable();
		}
	}

	m_fcFlags.SetFlag(UE_ACTIVE_FLAG, (unsigned int)_value);
}

// === SetName
//  IN : string - Name for the UIElement to use.
//  OUT : void
//
//  Sets the Name of the UIElement.
void UIElement::SetName(std::string _name)
{
	m_sName = _name;
}

// === SetTag
//  IN : string - Tag for the UIElement to use.
//  OUT : void
//
//  Sets the Tag of the UIElement.
void UIElement::SetTag(std::string _tag)
{
	m_sTag = _tag;
}
// ================ //