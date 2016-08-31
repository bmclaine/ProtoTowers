#include "UIComponents\UIDropdown.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIText.h"
#include "UIComponents\UIEventSystem.h"
#include "Assets\Texture.h"
#include "EventSystem.h"

#include "../../Interface/Public/Engine.h"
#include "../../Interface/Public/ModuleInterface.h"

#include <dwrite.h>
#include <sstream>

UIDropdown::UIDropdown(UIElement* _owner) : UISelectable(_owner)
{
	m_vValues.reserve(15);

	m_pFunciton = nullptr;

	m_pDownArrowUnselected = new Texture();
	m_pDownArrowUnselected->SetImage(Inferno::GetImage(L"UI_DropdownArrow_Unselected_Image.dds"));
	m_pDownArrowHovered = new Texture();
	m_pDownArrowHovered->SetImage(Inferno::GetImage(L"UI_DropdownArrow_Hovered_Image.dds"));;
	m_pDownArrowSelected = new Texture();
	m_pDownArrowSelected->SetImage(Inferno::GetImage(L"UI_DropdownArrow_Selected_Image.dds"));;
	m_pDropdownBox = new Texture();
	m_pSelectedFieldBackground = new Texture();
	m_pSelectedFieldBackground->SetImage(Inferno::GetImage(L"Fill.dds"));
	m_pSelectionFieldForeground = new Texture();
	m_pSelectionFieldForeground->SetImage(Inferno::GetImage(L"Fill.dds"));
	m_pSelectionFieldBackground = new Texture();
	m_pSelectionFieldBackground->SetImage(Inferno::GetImage(L"Fill.dds"));

	m_pArrowRenderer = nullptr;
	m_pSelectionBoxBackground = nullptr;
	m_pSelectionBoxForeground = nullptr;
	m_pSelectedText = nullptr;
	m_pSelectionText = nullptr;

	m_uiLayer = 0;
	m_fFontSize = 12.0f;
	m_taAlignment = LEADING;
	m_fCellSize = 0.0f;
	m_iSelectedElement = -1;
	m_iSelectionElement = -1;
	m_i2MousePos = { 0, 0 };

	m_bIsSelected = false;
}

UIDropdown::~UIDropdown()
{
	delete m_pDownArrowUnselected;
	delete m_pDownArrowHovered;
	delete m_pDownArrowSelected;
	delete m_pDropdownBox;
	delete m_pSelectedFieldBackground;
	delete m_pSelectionFieldForeground;
	delete m_pSelectionFieldBackground;

	EventSystem::GetInstance()->UnregisterFromEvent("SelectionUp", this, std::bind(&UIDropdown::MoveSelectionUp, this));
	EventSystem::GetInstance()->UnregisterFromEvent("SelectionDown", this, std::bind(&UIDropdown::MoveSelectionDown, this));
	EventSystem::GetInstance()->UnregisterFromEvent("ResolutionChange", this, BindEventFunction(&UIDropdown::OnResolutionChange, this));
//	InfernoEngine_UI::UnregisterFromCanvasScaleChange(m_pUIElement->GetCanvasName(), this);

	InfernoEngine_Input::UnregisterEventFromActionBinding("SecondaryInteract", this, BindActionFunction(&UIDropdown::OnExit, this));
}

void UIDropdown::OnSelect()
{
	if (m_bIsDisabled)
		return;

	if (m_pArrowRenderer && !m_bIsSelected)
		m_pArrowRenderer->SetTexture(m_pDownArrowHovered);
}
void UIDropdown::OnPressed()
{
	if (m_bIsDisabled)
		return;

	if (m_pArrowRenderer)
	{
		// Check to see if this is the first selection or second
		if (!m_bIsSelected)
		{
			// Set the dropdown to expanded state
			m_pArrowRenderer->SetTexture(m_pDownArrowSelected);
			m_pSelectionBoxBackground->SetActive(true);
			m_bIsSelected = true;
			m_pUIElement->GetEventSystem()->LockControl(true);

			EventSystem::GetInstance()->SendEventNow("UILocked", nullptr);
		}
		else
		{
			// Check to see if the selected element is in bounds
			if (m_iSelectionElement >= 0 && m_iSelectionElement < m_vValues.size())
			{
				// Check to see if the selected element is different from the previous one
				if (m_pSelectedText->GetStringW() != m_vValues[m_iSelectionElement])
				{
					// Set the new element
					m_pSelectedText->SetText(m_vValues[m_iSelectionElement]);
					m_iSelectedElement = m_iSelectionElement;

					// If a function was set to call, call it
					if (m_pFunciton)
						m_pFunciton();
				}
			}
			else
			{
				m_iSelectionElement = m_iSelectedElement;
				RectTransform* transform = m_pSelectionElement->GetTransform();
				transform->SetPosition(Vector2(0, m_iSelectionElement * m_fCellSize + (transform->GetBounds().y - 1)));
			}

			// Reset the dropdown to minimized state
			m_pArrowRenderer->SetTexture(m_pDownArrowHovered);
			m_pSelectionBoxBackground->SetActive(false);
			m_bIsSelected = false;
			m_pUIElement->GetEventSystem()->LockControl(false);

			EventSystem::GetInstance()->QueueEvent("UIUnlocked", nullptr);
		}
	}
}
void UIDropdown::OnExit(ACTION_STATUS status)
{
	if (m_bIsDisabled || status != ACTION_STATUS::AS_PRESSED || !m_bIsSelected)
		return;

	if (m_pArrowRenderer)
	{
		m_pArrowRenderer->SetTexture(m_pDownArrowHovered);
		m_pSelectionBoxBackground->SetActive(false);

		m_pSelectionBoxBackground->SetActive(false);
		m_bIsSelected = false;
		m_pUIElement->GetEventSystem()->LockControl(false);

		EventSystem::GetInstance()->QueueEvent("UIUnlocked", nullptr);

		m_iSelectionElement = m_iSelectedElement;
		RectTransform* transform = m_pSelectionElement->GetTransform();
		transform->SetPosition(Vector2(0, m_iSelectionElement * m_fCellSize + (transform->GetBounds().y - 1)));
	}
}
void UIDropdown::OnDeselect()
{
	if (m_bIsDisabled)
		return;

	if (m_pArrowRenderer && !m_bIsSelected)
		m_pArrowRenderer->SetTexture(m_pDownArrowUnselected);
}

// === OnEnabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
void UIDropdown::OnEnabled()
{
	if (m_pArrowRenderer)
	{
		m_pArrowRenderer->SetTexture(m_pDownArrowUnselected);
		m_pBoxRenderer->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}
}
// === OnDisabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is disabled
void UIDropdown::OnDisabled()
{
	if (m_pArrowRenderer)
	{
		m_pArrowRenderer->SetTexture(m_pDownArrowHovered);
		m_pBoxRenderer->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
	}
}
void UIDropdown::OnMouseHover(POINT _cursorPosition, POINT _displacement)
{
	// Only perform our selection checks if we have been clicked on
	if (m_bIsSelected && !m_bIsDisabled && m_vValues.size())
	{
		m_i2MousePos = _cursorPosition;

		// Check to see if the cursor is out of bounds
		if (_cursorPosition.x < m_f4SelectionRegion.y || _cursorPosition.x > m_f4SelectionRegion.w ||
			_cursorPosition.y < m_f4SelectionRegion.x || _cursorPosition.y > m_f4SelectionRegion.z)
		{
			m_iSelectionElement = -1;
			return;
		}

		float yScale = m_pSelectionBoxForeground->GetTransform()->GetCanvasScale().y;
		int position = _cursorPosition.y - (int)m_f4SelectionRegion.x;
		m_iSelectionElement = int(position / (m_fCellSize * yScale));

		RectTransform* transform = m_pSelectionElement->GetTransform();
		transform->SetPosition(Vector2(0, m_iSelectionElement * m_fCellSize + (transform->GetBounds().y - 1)));
	}
}
void UIDropdown::OnResolutionChange(void* pData)
{
	m_f4SelectionRegion = m_pSelectionBoxForeground->GetTransform()->GetScreenRectangle();
	Vector2 CanvasScale = m_pSelectionBoxForeground->GetTransform()->GetCanvasScale();
	m_f4SelectionRegion.x *= CanvasScale.y; m_f4SelectionRegion.z *= CanvasScale.y;
	m_f4SelectionRegion.y *= CanvasScale.x; m_f4SelectionRegion.w *= CanvasScale.x;
}

// === Init
//  IN : void
//  OUT : void
//
//  Function that gets called to initialize the component. Can be overrriden by any derived UIComponents.
void UIDropdown::Init()
{
	
}
// === PostInit
//  IN : void
//  OUT : void
//
//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
void UIDropdown::PostInit()
{
	//InfernoEngine_UI::RegisterForCanvasScaleChange(*m_pUIElement->GetCanvasName(), this, std::bind(&UIDropdown::OnResolutionChange, this));

	// Register for the event system fo the UIElement
	UIEventSystem* eventSystem = m_pUIElement->GetEventSystem();
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONDISABLED, BindFunction(&UIDropdown::OnDisabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONENABLED, BindFunction(&UIDropdown::OnEnabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, BindFunction(&UIDropdown::OnSelect));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, BindFunction(&UIDropdown::OnDeselect));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, BindFunction(&UIDropdown::OnPressed));
	//eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONRELEASED, BindFunction(&UIDropdown::OnDeselect));
	eventSystem->RegisterForMouseHover(BindMouseMovementFunction(&UIDropdown::OnMouseHover, this));

	EventSystem::GetInstance()->RegisterForEvent("SelectionUp", this, std::bind(&UIDropdown::MoveSelectionUp, this));
	EventSystem::GetInstance()->RegisterForEvent("SelectionDown", this, std::bind(&UIDropdown::MoveSelectionDown, this));
	EventSystem::GetInstance()->RegisterForEvent("ResolutionChange", this, BindEventFunction(&UIDropdown::OnResolutionChange, this));
//	InfernoEngine_UI::RegisterForCanvasScaleChange(m_pUIElement->GetCanvasName(), this, std::bind(&UIDropdown::OnResolutionChange, this));

	InfernoEngine_Input::RegisterEventToActionBinding("SecondaryInteract", this, BindActionFunction(&UIDropdown::OnExit, this));

	OnEnabled();

	//UISelectable::PostInit();

	Vector2 bounds = m_pRectTransform->GetTotalBounds();
	std::wstring imageName = L"Dropdown" + std::to_wstring(bounds.x) + L"x" + std::to_wstring(bounds.y);
	int dropboxImageID = Inferno::GetImage(imageName.c_str());

	if (dropboxImageID == -1)
		m_pDropdownBox->SetImage(InfernoEngine_Graphics::CreateSolidColorImage(imageName.c_str(), (UINT)bounds.x, (UINT)bounds.y, 2));
	else
		m_pDropdownBox->SetImage(dropboxImageID);

	m_pBoxRenderer = m_pUIElement->AddComponent<UIRenderer>();
	m_pBoxRenderer->SetTexture(m_pDropdownBox);
	m_pBoxRenderer->SetLayer(m_uiLayer);

	UIElement* DropdownArrow = new UIElement();
	RectTransform* transform = DropdownArrow->GetTransform();
	transform->SetParent(m_pRectTransform);
	transform->SetBounds(Vector2(bounds.y - 4, bounds.y - 4));
	transform->SetPosition(Vector2(m_pRectTransform->GetBounds().x - transform->GetBounds().x - 2, 0));

	m_pArrowRenderer = DropdownArrow->AddComponent<UIRenderer>();
	m_pArrowRenderer->SetTexture(m_pDownArrowUnselected);
	m_pArrowRenderer->SetLayer(m_uiLayer);

	UIElement* SelectedText = new UIElement();
	transform = SelectedText->GetTransform();
	transform->SetParent(m_pRectTransform);
	transform->SetBounds(Vector2(bounds.x - 4 - (bounds.y - 4), bounds.y - 4));
	transform->SetPosition(Vector2(-(bounds.y - 4) * 0.5f, 0));

	m_pSelectedText = SelectedText->AddComponent<UIText>();
	m_pSelectedText->SetText(m_wsStartText);
	m_pSelectedText->SetLayer(m_uiLayer + 1);
	m_pSelectedText->SetFontSize(m_fFontSize);
	m_pSelectedText->SetParagraphAlignment(PARAGRAPH_CENTER);
	m_pSelectedText->SetAlignment(m_taAlignment);
	m_pSelectedText->SetColor(Color(0, 0, 0, 1));

	m_fCellSize = m_fFontSize + (m_fFontSize / 6.5f);
	m_pSelectionBoxBackground = new UIElement();
	m_pSelectionBoxBackground->SetActive(false);
	transform = m_pSelectionBoxBackground->GetTransform();
	transform->SetParent(m_pRectTransform);
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetBounds(Vector2(m_pRectTransform->GetTotalBounds().x, 4 + (m_vValues.size() > 0 ? m_vValues.size() : 1) * (m_fCellSize)));
	transform->SetPosition(Vector2(0, transform->GetBounds().y));

	UIRenderer* image = m_pSelectionBoxBackground->AddComponent<UIRenderer>();
	image->SetTexture(m_pSelectionFieldBackground);
	image->SetColor(Color(0, 0, 0, 1));
	image->SetLayer(m_uiLayer + 9);

	m_pSelectionBoxForeground = new UIElement();
	transform = m_pSelectionBoxForeground->GetTransform();
	transform->SetParent(m_pSelectionBoxBackground->GetTransform());
	transform->SetBounds(Vector2(m_pRectTransform->GetTotalBounds().x - 4, (m_vValues.size() > 0 ? m_vValues.size() : 1) * (m_fCellSize)));

	image = m_pSelectionBoxForeground->AddComponent<UIRenderer>();
	image->SetTexture(m_pSelectionFieldForeground);
	image->SetColor(Color(1, 1, 1, 1));
	image->SetLayer(m_uiLayer + 10);

	{
		std::wstringstream wstream;
		for (size_t i = 0; i < m_vValues.size(); i++)
		{
			if (i != 0)
				wstream << L'\n';

			wstream << m_vValues[i];
		}

		m_pSelectionText = m_pSelectionBoxForeground->AddComponent<UIText>();
		m_pSelectionText->SetText(wstream.str());
		m_pSelectionText->SetFontSize(m_fFontSize);
		m_pSelectionText->SetColor(Color(0, 0, 0, 1));
		m_pSelectionText->SetLayer(m_uiLayer + 12);
		m_pSelectionText->SetAlignment(m_taAlignment);
	}

	// x - top, y - left, z - bottom, w - right
	m_f4SelectionRegion = m_pSelectionBoxForeground->GetTransform()->GetScreenRectangle();
	Vector2 CanvasScale = m_pSelectionBoxForeground->GetTransform()->GetCanvasScale();
	m_f4SelectionRegion.x *= CanvasScale.y; m_f4SelectionRegion.z *= CanvasScale.y;
	m_f4SelectionRegion.y *= CanvasScale.x; m_f4SelectionRegion.w *= CanvasScale.x;

	if (m_vValues.size())
	{
		for (size_t i = 0; i < m_vValues.size(); i++)
		{
			if (m_vValues[i] == m_pSelectedText->GetStringW())
			{
				m_iSelectionElement = m_iSelectedElement = (int)i;
				break;
			}
		}
	}

	m_pSelectionElement = new UIElement();
	m_pSelectionElement->SetActive((m_iSelectedElement >= 0) ? true : false);
	transform = m_pSelectionElement->GetTransform();
	transform->SetParent(m_pSelectionBoxForeground->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(m_pRectTransform->GetTotalBounds().x - 4, m_fCellSize));
	transform->SetPosition(Vector2(0, (m_fFontSize + (m_fFontSize / 6.0f)) * m_iSelectedElement + (transform->GetBounds().y - 1)));

	image = m_pSelectionElement->AddComponent<UIRenderer>();
	image->SetTexture(m_pSelectedFieldBackground);
	image->SetColor(Color(119.0f / 255.0f, 126.0f / 255.0f, 189.0f / 255.0f, 1.0f));
	image->SetLayer(m_uiLayer + 11);
}
// === Update
//  IN : void
//  OUT : void
//
//  Function that gets called every frame. Can be overriden by any derived UIComponents.
void UIDropdown::Update()
{

}

UISelectable* UIDropdown::GetUpSelectable() const
{
	if (!m_bIsSelected)
		return m_pUpSelectable;
	else
	{
		EventSystem::GetInstance()->SendEventNow("SelectionUp", nullptr);
		return nullptr;
	}
}
UISelectable* UIDropdown::GetDownSelectable() const
{
	if (!m_bIsSelected)
		return m_pDownSelectable;
	else
	{
		EventSystem::GetInstance()->SendEventNow("SelectionDown", nullptr);
		return nullptr;
	}
}
UISelectable* UIDropdown::GetRightSelectable() const
{
	return m_pRightSelectable;
}
UISelectable* UIDropdown::GetLeftSelectable() const
{
	return m_pLeftSelectable;
}
std::wstring UIDropdown::GetSelectedValue() const
{
	return m_pSelectedText->GetStringW();
}
int UIDropdown::GetSelectionID() const
{
	return m_iSelectedElement;
}

void UIDropdown::SetFunctionToCallOnSelectionChange(SelectionFunction function)
{
	m_pFunciton = function;
}
void UIDropdown::SetStartingValue(std::wstring value)
{
	m_wsStartText = value;
}
void UIDropdown::SetStartingValue(UINT index)
{
	if (index < m_vValues.size())
		m_wsStartText = m_vValues[index];
}
void UIDropdown::SetSelectedValue(std::wstring value)
{
	if (m_pSelectedText)
		m_pSelectedText->SetText(value);
}
void UIDropdown::SetSelectedValue(UINT index)
{
	if (m_pSelectedText && index < m_vValues.size())
	{
		m_pSelectedText->SetText(m_vValues[index]);
		m_iSelectedElement = index;
	}
}
void UIDropdown::SetSelectableValues(std::vector<std::wstring>& values)
{
	m_vValues = values;

	if (m_pSelectionBoxBackground && m_pSelectionBoxForeground && m_pSelectionText)
	{
		RectTransform* transform = m_pSelectionBoxBackground->GetTransform();
		transform->SetBounds(Vector2(m_pRectTransform->GetTotalBounds().x, 4 + (m_vValues.size() > 0 ? m_vValues.size() : 1) * (m_pSelectionText->GetFontSize() + 2)));
		transform->SetPosition(Vector2(0, transform->GetBounds().y));

		transform = m_pSelectionBoxForeground->GetTransform();
		transform->SetBounds(Vector2(m_pRectTransform->GetTotalBounds().x - 4, (m_vValues.size() > 0 ? m_vValues.size() : 1) * (m_pSelectionText->GetFontSize() + 2)));

		m_pSelectionText->ResizeFontTexture();

		// x - top, y - left, z - bottom, w - right
		m_f4SelectionRegion = m_pSelectionBoxForeground->GetTransform()->GetScreenRectangle();
		Vector2 CanvasScale = m_pSelectionBoxForeground->GetTransform()->GetCanvasScale();
		m_f4SelectionRegion.x *= CanvasScale.y; m_f4SelectionRegion.z *= CanvasScale.y;
		m_f4SelectionRegion.y *= CanvasScale.x; m_f4SelectionRegion.w *= CanvasScale.x;

		std::wstringstream wstream;
		for (size_t i = 0; i < m_vValues.size(); i++)
		{
			if (i != 0)
				wstream << L'\n';

			wstream << m_vValues[i];
		}

		m_pSelectionText->SetText(wstream.str());
	}
}
void UIDropdown::SetLayer(UINT layer)
{
	m_uiLayer = layer;
}
void UIDropdown::SetFontSize(float fontSize)
{
	m_fFontSize = fontSize;

	if (m_pSelectedText && m_pSelectionText)
	{
		m_pSelectedText->SetFontSize(fontSize);
		m_pSelectionText->SetFontSize(fontSize);

		m_fCellSize = m_fFontSize + (m_fFontSize / 6.5f);
		m_pSelectionElement->GetTransform()->SetBounds(Vector2(m_pRectTransform->GetTotalBounds().x - 4, m_fCellSize));
	}
}
void UIDropdown::SetFontAlignment(TextAlignment alignment)
{
	m_taAlignment = alignment;

	if (m_pSelectedText && m_pSelectionText)
	{
		m_pSelectedText->SetAlignment(alignment);
		m_pSelectionText->SetAlignment(alignment);
	}
}

void UIDropdown::MoveSelectionUp()
{
	if (m_iSelectionElement > 0 && m_bIsSelected)
	{
		m_iSelectionElement--;

		RectTransform* transform = m_pSelectionElement->GetTransform();
		transform->SetPosition(Vector2(0, m_fCellSize * m_iSelectionElement + (transform->GetBounds().y - 1)));
	}
}
void UIDropdown::MoveSelectionDown()
{
	if (m_iSelectionElement < m_vValues.size() - 1 && m_vValues.size() && m_bIsSelected)
	{
		m_iSelectionElement++;

		RectTransform* transform = m_pSelectionElement->GetTransform();
		transform->SetPosition(Vector2(0, m_fCellSize * m_iSelectionElement + (transform->GetBounds().y - 1)));
	}
}