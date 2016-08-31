#include "UIComponents\UICheckbox.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIEventSystem.h"
#include "Assets\Texture.h"

#include "../../Interface/Public/Engine.h"
#include "InputManager.h"

UICheckbox::UICheckbox(UIElement* _owner) : UISelectable(_owner)
{
	m_pBoxTexture = new Texture();
	m_pBoxTexture->SetImage(Inferno::GetImage(L"UI_Checkbox_Image.dds"));
	m_pCheckTexture = new Texture();
	m_pCheckTexture->SetImage(Inferno::GetImage(L"UI_Check_Image.dds"));

	m_pBoxRenderer = nullptr;
	m_pCheckRenderer = nullptr;

	UINT m_uiLayer = 0;
	bool m_bChecked = false;
}

UICheckbox::~UICheckbox()
{
	delete m_pBoxTexture;
	delete m_pCheckTexture;
}

void UICheckbox::OnSelect()
{
	if (m_bIsDisabled)
		return;

	if (m_pBoxRenderer)
		m_pBoxRenderer->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
}
void UICheckbox::OnPressed()
{
	if (m_bIsDisabled)
		return;

	if (m_pBoxRenderer)
	{
		if (InputManager::UsingController())
		{
			if (m_pCheckRenderer)
			{
				m_pBoxRenderer->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
				m_bChecked = !m_bChecked;

				m_pCheckRenderer->SetEnabled(m_bChecked);
			}
		}
		else
		{
			m_pBoxRenderer->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
		}
	}
}
void UICheckbox::OnRelease()
{
	if (m_bIsDisabled)
		return;

	if (m_pBoxRenderer)
	{
		m_pBoxRenderer->SetColor(Color(0.8f, 0.8f, 0.8f, 1.0f));
		m_bChecked = !m_bChecked;

		if (m_pCheckRenderer)
			m_pCheckRenderer->SetEnabled(m_bChecked);
	}
}
void UICheckbox::OnDeselect()
{
	if (m_bIsDisabled)
		return;

	if (m_pBoxRenderer)
		m_pBoxRenderer->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
}

// === OnEnabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
void UICheckbox::OnEnabled()
{
	if (m_pBoxRenderer)
		m_pBoxRenderer->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
}
// === OnDisabled
//  IN : void
//  OUT : void
//
//  Event Function that is triggered when the Button is disabled
void UICheckbox::OnDisabled()
{
	if (m_pBoxRenderer)
		m_pBoxRenderer->SetColor(Color(0.3f, 0.3f, 0.3f, 1.0f));
}

// === PostInit
//  IN : void
//  OUT : void
//
//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
void UICheckbox::PostInit()
{
	// Register for the event system fo the UIElement
	UIEventSystem* eventSystem = UISelectable::m_pUIElement->GetEventSystem();
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONDISABLED, BindFunction(&UICheckbox::OnDisabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONENABLED, BindFunction(&UICheckbox::OnEnabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, BindFunction(&UICheckbox::OnSelect));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, BindFunction(&UICheckbox::OnEnabled));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, BindFunction(&UICheckbox::OnPressed));
	eventSystem->RegisterEvent(UI_EVENTS::UI_EVENT_ONRELEASED, BindFunction(&UICheckbox::OnRelease));

	UISelectable::PostInit();

	m_pBoxRenderer = m_pUIElement->AddComponent<UIRenderer>();
	m_pBoxRenderer->SetLayer(m_uiLayer);
	m_pBoxRenderer->SetTexture(m_pBoxTexture);

	m_pCheckRenderer = m_pUIElement->AddComponent<UIRenderer>();
	m_pCheckRenderer->SetLayer(m_uiLayer);
	m_pCheckRenderer->SetTexture(m_pCheckTexture);
	m_pCheckRenderer->SetEnabled(m_bChecked);
}
// === Update
//  IN : void
//  OUT : void
//
//  Function that gets called every frame. Can be overriden by any derived UIComponents.
void UICheckbox::Update() {}

bool UICheckbox::IsChecked() const
{
	return m_bChecked;
}

void UICheckbox::SetChecked(bool checked)
{
	m_bChecked = checked;

	if (m_pCheckRenderer)
		m_pCheckRenderer->SetEnabled(checked);
}
void UICheckbox::SetLayer(UINT layer)
{
	m_uiLayer = layer;
}