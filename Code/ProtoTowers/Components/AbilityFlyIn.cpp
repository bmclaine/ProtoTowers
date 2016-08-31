#include "stdafx.h"
#include "AbilityFlyIn.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\RectTransform.h"
#include "UIComponents\UICanvas.h"

AbilityFlyIn::AbilityFlyIn(UIElement* _owner) : UIComponent(_owner)
{
	m_bMove = false;
	m_pAbilityTexture = nullptr;
	m_pAbilityRenderer = nullptr;
	m_bWaiting = false;
}

AbilityFlyIn::~AbilityFlyIn()
{
	OnDisable();
}

void AbilityFlyIn::OnEnable()
{
	UIComponent::OnEnable();
	if (m_pAbilityRenderer != nullptr)
	{
		m_pAbilityRenderer->SetEnabled(false);
	}
}

void AbilityFlyIn::OnDisable()
{
	UIComponent::OnDisable();
}

void AbilityFlyIn::Update()
{
	if (m_bWaiting && m_itWaitTimer.IsFinished())
	{
		m_bMove = true;
		m_itDisplayTimer.SetTime(1.0f);
		m_itDisplayTimer.Reset();
		m_bWaiting = false;
	}
	if (m_bMove)
	{
		UpdateFlyIn();
	}
	if (m_bMove && m_itDisplayTimer.IsFinished())
	{
		m_bMove = false;
		m_pAbilityRenderer->SetEnabled(false);
	}
}

void AbilityFlyIn::SetStartPosition(Vector2 _start)
{
	m_vStartPosition = _start;
}

void AbilityFlyIn::SetEndPostion(Vector2 _end)
{
	m_vEndPosition = _end;
}

void AbilityFlyIn::Reset()
{
	m_pAbilityRenderer->GetTransform()->SetPosition(m_vStartPosition);
	m_bMove = false;
}

void AbilityFlyIn::UpdateFlyIn()
{
	Vector2 displacement = m_vEndPosition - m_vStartPosition;
	float ratio = (m_itDisplayTimer.GetSetTimeSeconds() - m_itDisplayTimer.TimeRemaining()) / m_itDisplayTimer.GetSetTimeSeconds();
	float newScale = (64 - 128) * ratio + 128;

	m_pAbilityRenderer->GetTransform()->SetBounds(Vector2(newScale, newScale));
	m_pAbilityRenderer->GetTransform()->SetPosition(displacement * ratio + m_vStartPosition);

}

void AbilityFlyIn::SetTexture(Texture * _image)
{
	m_pAbilityTexture = _image;
}

void AbilityFlyIn::SetTextureImage(int id)
{
	m_pAbilityTexture->SetImage(id);
}

void AbilityFlyIn::SetRenderer(UIRenderer * renderer)
{
	m_pAbilityRenderer = renderer;
}

void AbilityFlyIn::SetMoving(bool _moving)
{
	m_bMove = _moving;
}

void AbilityFlyIn::DisplayAbilityIcon(int ability)
{
	switch (ability)
	{
	case 0:
		m_pAbilityTexture->SetImage(Inferno::GetImage(L"StunGernadeAbilityIcon.dds")); 
		break;
	case 1:
		m_pAbilityTexture->SetImage(Inferno::GetImage(L"PlayerShield.dds"));
		break;
	case 2:
		m_pAbilityTexture->SetImage(Inferno::GetImage(L"WeaponBuff.dds"));
		break;
	case 3:
		m_pAbilityTexture->SetImage(Inferno::GetImage(L"MinionBuff.dds"));
		break;
	default:
		break;
	}
	Reset();
	m_bWaiting = true;
	m_vEndPosition = Vector2(ability * 70.0f + 51, 320);
	m_pAbilityRenderer->GetTransform()->SetBounds(Vector2(128, 128));
	Vector2 dimensions = Inferno_UI::GetCanvasByName("ExtendedHUD")->GetTargetResolution();
	m_vStartPosition = Vector2(dimensions.x * 0.5f, .0f);
	m_pAbilityRenderer->GetTransform()->SetPosition(m_vStartPosition);
	m_pAbilityRenderer->SetEnabled(true);
	m_itWaitTimer.SetTime(.5f);
	m_itWaitTimer.Reset();


}