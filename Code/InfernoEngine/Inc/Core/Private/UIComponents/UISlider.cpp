#include "UIComponents\UISlider.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIEventSystem.h"
#include "UIComponents\RectTransform.h"
#include "Assets\Texture.h"

#include "../../Interface/Public/Engine.h"
#include "InputManager.h"
#include "EventSystem.h"

UISlider::UISlider(UIElement* _owner) : UISelectable(_owner)
{
	m_cBackgroundColor = Color(1, 1, 1, 1);
	m_cForegroundColor = Color(0, 0, 0, 1);
	m_cSelectionRegionHoveredColor = Color(0, 0, 0, 0.5f);
	m_cSelectionRegionUnselectedColor = Color(0, 0, 0, 0);
	m_cSelectionRegionSelectedColor = Color(0, 0, 0, 1);

	m_pBackgroundImage = new Texture();
	m_pBackgroundImage->SetImage(Inferno::GetImage(L"Fill.dds"));
	m_pForegroundImage = new Texture();
	m_pForegroundImage->SetImage(Inferno::GetImage(L"Fill.dds"));
	m_pSelectionRegionImage = new Texture();
	m_pSelectionRegionImage->SetImage(Inferno::GetImage(L"Fill.dds"));

	m_pFillImageOne = nullptr;
	m_pFillImageTwo = nullptr;
	m_pSelectionRegion = nullptr;

	m_pFunc = nullptr;

	m_uiLayer = 0;
	m_fFillAmount = 0.0f;
	m_bSelected = false;
	m_bFillRight = false;
}

UISlider::~UISlider()
{
	delete m_pBackgroundImage;
	delete m_pForegroundImage;
	delete m_pSelectionRegionImage;

	EventSystem::GetInstance()->UnregisterFromEvent("SliderFillRight", this, std::bind(&UISlider::MoveSliderRight, this));
	EventSystem::GetInstance()->UnregisterFromEvent("SliderFillLeft", this, std::bind(&UISlider::MoveSliderLeft, this));
}

void UISlider::MoveSliderRight()
{
	if (m_bSelected)
	{
		// Calculate our new fill amount
		m_fFillAmount = Clamp(0.0f, 1.0f, m_fFillAmount + (m_bFillRight ? -0.01f : 0.01f));
		m_pFillImageOne->SetImageFill(1.0f - m_fFillAmount);
		m_pFillImageTwo->SetImageFill(m_fFillAmount);

		// If we have an attached function, call it
		if (m_pFunc)
			m_pFunc(&m_fFillAmount);
	}
}
void UISlider::MoveSliderLeft()
{
	if (m_bSelected)
	{
		// Calculate our new fill amount
		m_fFillAmount = Clamp(0.0f, 1.0f, m_fFillAmount + (m_bFillRight ? 0.01f : -0.01f));
		m_pFillImageOne->SetImageFill(1.0f - m_fFillAmount);
		m_pFillImageTwo->SetImageFill(m_fFillAmount);

		// If we have an attached function, call it
		if (m_pFunc)
			m_pFunc(&m_fFillAmount);
	}
}

void UISlider::OnSelect()
{
	if (m_bIsDisabled)
		return;

	if (m_pSelectionRegion)
		m_pSelectionRegion->SetColor(m_cSelectionRegionHoveredColor);
}
void UISlider::OnPressed()
{
	if (m_bIsDisabled)
		return;

	if (m_pSelectionRegion)
	{
		if (InputManager::UsingController())
		{
			if (m_bSelected)
				m_pSelectionRegion->SetColor(m_cSelectionRegionHoveredColor);
			else
				m_pSelectionRegion->SetColor(m_cSelectionRegionSelectedColor);
		}
		else
			m_pUIElement->GetEventSystem()->LockControl(true);
			
		m_bSelected = !m_bSelected;
	}
}
void UISlider::OnRelease()
{
	if (m_bIsDisabled)
		return;

	if (m_pSelectionRegion)
	{
		m_pSelectionRegion->SetColor(m_cSelectionRegionHoveredColor);
		m_bSelected = false;
		m_pUIElement->GetEventSystem()->LockControl(false);
	}
}
void UISlider::OnDeselect()
{
	if (m_bIsDisabled)
		return;

	if (m_pSelectionRegion)
		m_pSelectionRegion->SetColor(m_cSelectionRegionUnselectedColor);

	m_bSelected = false;
}
void UISlider::OnMouseHover(POINT _cursorPosition, POINT _displacement)
{
	if (m_pFillImageOne && m_pFillImageTwo && m_bSelected)
	{
		// x - top, y - left, z - bottom, w - right
		RectTransform* transform = m_pFillImageOne->GetTransform();
		Vector4 rect = transform->GetScreenRectangle();
		float Scale = transform->GetCanvasScale().x;// *transform->GetScale().x;
		rect.w *= Scale; rect.y *= Scale;

		// Get the mouses X position relative to the side that would be considered 0 fill
		int mouseX = (m_bFillRight) ? int(rect.w - _cursorPosition.x) : int(_cursorPosition.x - rect.y);

		// Calculate the fill ratio based on the mouses position
		float fill = Clamp(0.0f, 1.0f, mouseX / (rect.w - rect.y));
		m_fFillAmount = fill;
		m_pFillImageOne->SetImageFill(1.0f - fill);
		m_pFillImageTwo->SetImageFill(fill);

		if (m_pFunc)
			m_pFunc(&m_fFillAmount);
	}
}

// === OnEnabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
void UISlider::OnEnabled()
{
	if (m_pFillImageOne && m_pFillImageTwo)
	{
		m_pFillImageOne->SetColor(m_cBackgroundColor);
		m_pFillImageTwo->SetColor(m_cBackgroundColor);
	}
}
// === OnDisabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is disabled
void UISlider::OnDisabled()
{
	if (m_pFillImageOne && m_pFillImageTwo)
	{
		m_pFillImageOne->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
		m_pFillImageTwo->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
	}
}

// === PostInit
//  IN : void
//  OUT : void
//
//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
void UISlider::PostInit()
{
	// Register for the event system fo the UIElement
	UIEventSystem* eventSystem = UISelectable::m_pUIElement->GetEventSystem();
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONDISABLED, BindFunction(&UISlider::OnDisabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONENABLED, BindFunction(&UISlider::OnEnabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, BindFunction(&UISlider::OnSelect));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, BindFunction(&UISlider::OnDeselect));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, BindFunction(&UISlider::OnPressed));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONRELEASED, BindFunction(&UISlider::OnRelease));
	eventSystem->RegisterForMouseHover(BindMouseMovementFunction(&UISlider::OnMouseHover, this));

	EventSystem::GetInstance()->RegisterForEvent("SliderFillRight", this, std::bind(&UISlider::MoveSliderRight, this));
	EventSystem::GetInstance()->RegisterForEvent("SliderFillLeft", this, std::bind(&UISlider::MoveSliderLeft, this));

	// Selection Region to show if the slider is unselected/hovered/selected
	// This will be the original size set and every other element will be scaled down
	m_pSelectionRegion = m_pUIElement->AddComponent<UIRenderer>();
	m_pSelectionRegion->SetTexture(m_pSelectionRegionImage);
	m_pSelectionRegion->SetColor(m_cSelectionRegionUnselectedColor);
	m_pSelectionRegion->SetLayer(m_uiLayer);

	// Fill Region
	UIElement* FillRegion = new UIElement();
	RectTransform* transform = FillRegion->GetTransform();
	transform->SetParent(m_pRectTransform);
	transform->SetBounds(m_pRectTransform->GetTotalBounds() - Vector2(4, 4)); // Scale down by 2 pixels on each side

	// Background Image
	m_pFillImageOne = FillRegion->AddComponent<UIRenderer>();
	m_pFillImageOne->SetEffect(m_bFillRight ? UIEffects::UI_FILL_EFFECT : UIEffects::UI_FILL_RIGHT_EFFECT);
	m_pFillImageOne->SetColor(m_cBackgroundColor);
	m_pFillImageOne->SetTexture(m_pBackgroundImage);
	m_pFillImageOne->SetImageFill(1.0f - m_fFillAmount);
	m_pFillImageOne->SetLayer(m_uiLayer);

	// Foreground Image
	m_pFillImageTwo = FillRegion->AddComponent<UIRenderer>();
	m_pFillImageTwo->SetEffect(m_bFillRight ? UIEffects::UI_FILL_RIGHT_EFFECT : UIEffects::UI_FILL_EFFECT);
	m_pFillImageTwo->SetColor(m_cForegroundColor);
	m_pFillImageTwo->SetTexture(m_pForegroundImage);
	m_pFillImageTwo->SetImageFill(m_fFillAmount);
	m_pFillImageTwo->SetLayer(m_uiLayer);
}
// === Update
//  IN : void
//  OUT : void
//
//  Function that gets called every frame. Can be overriden by any derived UIComponents.
void UISlider::Update()
{

}

UISelectable* UISlider::GetUpSelectable() const
{
	if (m_bSelected)
		return nullptr;

	return m_pUpSelectable;
}
UISelectable* UISlider::GetDownSelectable() const
{
	if (m_bSelected)
		return nullptr;

	return m_pDownSelectable;
}
UISelectable* UISlider::GetRightSelectable() const
{
	if (m_bSelected)
	{
		EventSystem::GetInstance()->SendEventNow("SliderFillRight", nullptr);
		return nullptr;
	}

	return m_pRightSelectable;
}
UISelectable* UISlider::GetLeftSelectable() const
{
	if (m_bSelected)
	{
		EventSystem::GetInstance()->SendEventNow("SliderFillLeft", nullptr);
		return nullptr;
	}

	return m_pLeftSelectable;
}
float UISlider::GetFill() const
{
	return m_fFillAmount;
}

void UISlider::SetOnFillChangeFunction(FillChangeFunction func)
{
	m_pFunc = func;
}
void UISlider::SetBackImageColor(Color color)
{
	m_cBackgroundColor = color;
}
void UISlider::SetFillImageColor(Color color)
{
	m_cForegroundColor = color;
}
void UISlider::SetUnselectedColor(Color color)
{
	m_cSelectionRegionUnselectedColor = color;
}
void UISlider::SetHoveredColor(Color color)
{
	m_cSelectionRegionHoveredColor = color;
}
void UISlider::SetSelectedColor(Color color)
{
	m_cSelectionRegionSelectedColor = color;
}
void UISlider::SetBackImage(Texture* image)
{
	m_pBackgroundImage->SetImage(image->GetImage());

	if (m_pFillImageOne)
		m_pFillImageOne->SetTexture(m_pBackgroundImage);
}
void UISlider::SetFrontImage(Texture* image)
{
	m_pForegroundImage->SetImage(image->GetImage());

	if (m_pFillImageTwo)
		m_pFillImageTwo->SetTexture(m_pForegroundImage);
}
void UISlider::SetSelectedImage(Texture* image)
{
	m_pSelectionRegionImage->SetImage(image->GetImage());

	if (m_pSelectionRegion)
		m_pSelectionRegion->SetTexture(m_pSelectionRegionImage);
}
void UISlider::SetFill(float fill)
{
	fill = Clamp(0.0f, 1.0f, fill);
	m_fFillAmount = fill;

	if (m_pFillImageOne && m_pFillImageTwo)
	{
		m_pFillImageOne->SetImageFill(1.0f - fill);
		m_pFillImageTwo->SetImageFill(fill);
	}
}
void UISlider::SetLayer(UINT layer)
{
	m_uiLayer = layer;
}
void UISlider::SetFillRight(bool fillRight)
{
	m_bFillRight = fillRight;
}