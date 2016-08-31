#include "stdafx.h"
#include "AbilityPanelScript.h"
#include "Ability.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIText.h"
#include "../InfernoUI.h"
#include "InputManager.h"

AbilityPanelScript::AbilityPanelScript(UIElement* _owner) : UIComponent(_owner)
{
	m_pController = nullptr;
	m_pAbilitySlotTexture = new Texture();
	m_pFillTexture = new Texture();

	InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&AbilityPanelScript::ControlChange, this));

	for (UINT i = 0; i < MAX_ABILITIES; i++)
	{
		m_pAbilityRenderers[i] = nullptr;
		m_pCooldownRenderers[i] = nullptr;
		m_pActiveTimeRenderers[i] = nullptr;
		m_pAbilities[i] = nullptr;
		m_pAbilityButtons[i] = nullptr;
		m_pAbilityButtonTextures[i] = new Texture();
	}
}

AbilityPanelScript::~AbilityPanelScript()
{
	SAFE_DELETE(m_pAbilitySlotTexture);
	SAFE_DELETE(m_pFillTexture);

	for (UINT i = 0; i < MAX_ABILITIES; i++)
		SAFE_DELETE(m_pAbilityButtonTextures[i]);

	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&AbilityPanelScript::ControlChange, this));
}

void AbilityPanelScript::ControlChange(bool usingController)
{
	if (m_pButtonElement && m_pKeyElement)
	{
		m_pButtonElement->SetActive(usingController);
		m_pKeyElement->SetActive(!usingController);
	}
}

// === Init
//  IN : void
//  OUT : void
//
//  Function that gets called to initialize the component. Can be overrriden by any derived UIComponents.
void AbilityPanelScript::Init()
{
	m_pAbilitySlotTexture->SetImage(Inferno::GetImage(L"AbilitySlot.dds"));
	m_pFillTexture->SetImage(Inferno::GetImage(L"Fill.dds"));
	bool usingController = InputManager::UsingController();

	m_pKeyElement = new UIElement();
	m_pKeyElement->SetActive(!usingController);
	RectTransform* transform = m_pKeyElement->GetTransform();
	transform->SetParent(m_pRectTransform);
	transform->SetAnchors(Anchors::ANCHOR_CENTERLEFT);
	transform->SetPosition(Vector2(51, 320));
	transform->SetBounds(Vector2(64, 64));
	m_pButtonElement = new UIElement();
	m_pButtonElement->SetActive(usingController);
	transform = m_pButtonElement->GetTransform();
	transform->SetParent(m_pRectTransform);
	transform->SetAnchors(Anchors::ANCHOR_CENTERLEFT);
	transform->SetPosition(Vector2(51, 320));
	transform->SetBounds(Vector2(64, 64));

	for (UINT i = 0; i < MAX_ABILITIES; i++)
	{
		UIElement* element = new UIElement();
		element->SetName("Ability Slot");
		RectTransform* transform = element->GetTransform();
		transform->SetParent(m_pRectTransform);
		transform->SetAnchors(Anchors::ANCHOR_CENTERLEFT);
		transform->SetBounds(Vector2(64, 64));
		transform->SetPosition(Vector2(i * 70.0f + 51, 320));
		Vector2 pos = transform->GetScreenPosition();

		UIRenderer* image = element->AddComponent<UIRenderer>();
		image->SetLayer(2);
		image->SetTexture(m_pAbilitySlotTexture);

		m_pCooldownRenderers[i] = element->AddComponent<UIRenderer>();
		m_pCooldownRenderers[i]->SetLayer(1);
		m_pCooldownRenderers[i]->SetTexture(m_pFillTexture);
		m_pCooldownRenderers[i]->SetEffect(UI_FILL_RADIAL);
		m_pCooldownRenderers[i]->SetRadialOrigin(ORIGIN_TOP);
		m_pCooldownRenderers[i]->SetImageFill(0.0f);
		m_pCooldownRenderers[i]->SetColor(Color(0.15f, 0.15f, 0.15f, 0.8f));
		m_pCooldownRenderers[i]->SetTexture(m_pFillTexture);

		m_pAbilityRenderers[i] = element->AddComponent<UIRenderer>();
		m_pAbilityRenderers[i]->SetLayer(0);
		m_pAbilityRenderers[i]->SetTexture(m_pFillTexture);
		m_pAbilityRenderers[i]->SetEnabled(false);

		UIElement* KeyText = new UIElement();
		transform = KeyText->GetTransform();
		transform->SetParent(m_pKeyElement->GetTransform());
		transform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		transform->SetPosition(Vector2(i * 70.0f + 14, 8));
		transform->SetBounds(Vector2(18, 18));

		m_pAbilityKeys[i] = KeyText->AddComponent<UIText>();
		m_pAbilityKeys[i]->SetText(std::to_string(i + 1));
		m_pAbilityKeys[i]->SetColor(Color(0, 0, 0, 1));
		m_pAbilityKeys[i]->SetAlignment(CENTER);
		m_pAbilityKeys[i]->SetParagraphAlignment(PARAGRAPH_CENTER);
		m_pAbilityKeys[i]->SetLayer(2);

		UIElement* ButtonImage = new UIElement();
		transform = ButtonImage->GetTransform();
		transform->SetParent(m_pButtonElement->GetTransform());
		transform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		transform->SetPosition(Vector2(i * 70.0f + 14, 8));
		transform->SetBounds(Vector2(18, 18));

		m_pAbilityButtons[i] = ButtonImage->AddComponent<UIRenderer>();
		m_pAbilityButtons[i]->SetTexture(m_pAbilityButtonTextures[i]);
		m_pAbilityButtons[i]->SetLayer(2);

		UIElement* ActiveBar = new UIElement();
		transform = ActiveBar->GetTransform();
		transform->SetParent(element->GetTransform());
		transform->SetBounds(Vector2(60, 6));
		transform->SetPosition(Vector2(0, -40));

		m_pActiveTimeRenderers[i] = ActiveBar->AddComponent<UIRenderer>();
		m_pActiveTimeRenderers[i]->SetTexture(m_pFillTexture);
		m_pActiveTimeRenderers[i]->SetEffect(UIEffects::UI_FILL_EFFECT);
		m_pActiveTimeRenderers[i]->SetImageFill(0.0f);
		m_pActiveTimeRenderers[i]->SetLayer(2);
		m_pActiveTimeRenderers[i]->SetEnabled(false);

		Inferno_UI::AddToView(element, "HUD");
	}

	Inferno_UI::AddToView(m_pKeyElement, "HUD");
	Inferno_UI::AddToView(m_pButtonElement, "HUD");
}
// === PostInit
//  IN : void
//  OUT : void
//
//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
void AbilityPanelScript::PostInit()
{

}

// === Update
//  IN : void
//  OUT : void
//
//  Function that gets called every frame. Can be overriden by any derived UIComponents.
void AbilityPanelScript::Update()
{
	for (UINT i = 0; i < MAX_ABILITIES; i++)
	{
		if (m_pAbilityRenderers[i]->IsEnabled() && m_pAbilities[i])
		{
			if (m_pAbilities[i]->GetCooldownRemaining())
			{
				if (!m_pCooldownRenderers[i]->IsEnabled())
					m_pCooldownRenderers[i]->SetEnabled(true);

				m_pCooldownRenderers[i]->SetImageFill(m_pAbilities[i]->GetCooldownRemaining() / m_pAbilities[i]->GetCooldown());
			}
			else
			{
				if (m_pCooldownRenderers[i]->IsEnabled())
					m_pCooldownRenderers[i]->SetEnabled(false);
			}

			if (m_pActiveTimeRenderers[i])
			{
				if (!m_itActiveTimers[i].IsFinished())
				{
					if (!m_pActiveTimeRenderers[i]->IsEnabled())
						m_pActiveTimeRenderers[i]->SetEnabled(true);

					m_pActiveTimeRenderers[i]->SetImageFill(m_itActiveTimers[i].TimeRemaining() / m_itActiveTimers[i].GetSetTimeSeconds());
				}
			}
			else
			{
				if (m_pActiveTimeRenderers[i]->IsEnabled())
					m_pActiveTimeRenderers[i]->SetEnabled(false);
			}
		}
	}
}

void AbilityPanelScript::SetPlayerController(PlayerController* controller)
{
	m_pController = controller;
}

void AbilityPanelScript::SetAbility(Ability* ability, UINT slot)
{
	if (slot < MAX_ABILITIES)
	{
		m_pAbilities[slot] = ability;

		if (ability && ability->GetTexture())
		{
			m_pAbilityRenderers[slot]->SetEnabled(true);
			m_pAbilityRenderers[slot]->SetTexture(ability->GetTexture());
			
			if (m_pActiveTimeRenderers[slot])
				m_pActiveTimeRenderers[slot]->SetColor(ability->GetActiveTimerColor());
		}
		else
		{
			m_pAbilityRenderers[slot]->SetEnabled(false);
			m_pCooldownRenderers[slot]->SetImageFill(0.0f);
			m_pCooldownRenderers[slot]->SetEnabled(false);
		}
	}
}
void AbilityPanelScript::SetAbilityActiveTime(float time, UINT slot)
{
	if (slot < MAX_ABILITIES)
		m_itActiveTimers[slot].SetTime(time);
}
void AbilityPanelScript::SetAbilityButtonTexture(Texture* texture, UINT slot)
{
	if (slot >= MAX_ABILITIES)
		return;

	if (texture && m_pAbilityButtonTextures[slot])
		m_pAbilityButtonTextures[slot]->SetImage(texture->GetImage());
}
void AbilityPanelScript::SetAbilityButtonTexture(UINT ID, UINT slot)
{
	if (slot >= MAX_ABILITIES)
		return;

	if (m_pAbilityButtonTextures[slot])
		m_pAbilityButtonTextures[slot]->SetImage(ID);
}