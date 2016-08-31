#include "stdafx.h"
#include "FadeAway.h"

#include "../InfernoUI.h"

// === Constructor / Destructor === //
FadeAway::FadeAway(UIElement* _owner) : UIComponent(_owner)
{
	m_pColor = new Color(0.0f, 0.0f, 0.0f, 1.0f);
	m_pFadeTexture = new Texture();
	m_pFadeTexture->SetImage(Inferno::GetImage(L"Fill.dds"));
	m_pImage = nullptr;
	m_fFadeOutTime = m_fFadeInTime = 0.0f;
}

FadeAway::~FadeAway()
{
	delete m_pColor;
	delete m_pFadeTexture;
}
// ================================ //

// === Interface === //
void FadeAway::PostInit()
{
	
}

void FadeAway::OnEnable()
{
	m_fFadeTimer = 0.0f;
	m_bFadingIn = false;

	if (m_pImage == nullptr) {
		GetUIElement()->SetName("Fade Away Effect");

		m_pImage = GetUIElement()->AddComponent<UIRenderer>();
		m_pImage->SetTexture(m_pFadeTexture);
		m_pImage->SetColor(*m_pColor);
		m_pImage->SetLayer(12);
	}

	m_pImage->SetEnabled(true);
}

void FadeAway::OnDisable()
{
	m_pImage->SetEnabled(false);
}

void FadeAway::Update()
{
	float ratio;

	if (m_bFadingIn) {
		m_fFadeTimer -= Time::DeltaTime;

		ratio = m_fFadeTimer / m_fFadeInTime;

		if (m_fFadeTimer <= 0.0f) {
			// === Finished
			this->SetEnabled(false);
		}
	}
	else {
		m_fFadeTimer += Time::DeltaTime;

		ratio = m_fFadeTimer / m_fFadeOutTime;

		if (m_fFadeTimer >= m_fFadeOutTime) {
			if (m_fFadeInTime != 0.0f) {
				// === Set to Fade Back in
				m_bFadingIn = true;
				m_fFadeTimer = m_fFadeInTime;
			}
			else {
				// === Finished
				this->SetEnabled(false);
			}
		}
	}

	m_pImage->SetColor(Color(m_pColor->Red, m_pColor->Green, m_pColor->Blue, m_pColor->Alpha * ratio));
}
// ================= //

// === Accessors === //
Color* FadeAway::GetColor() const
{
	return m_pColor;
}
// ================= //

// === Mutators === //
void FadeAway::SetFadeColor(float _r, float _g, float _b, float _a)
{
	m_pColor->Red = _r;
	m_pColor->Green = _g;
	m_pColor->Blue = _b;
	m_pColor->Alpha = _a;

	if (m_pImage != nullptr) {
		m_pImage->SetColor(*m_pColor);
	}
}

void FadeAway::SetFadeTime(float _fadeOutTime, float _fadeInTime)
{
	m_fFadeOutTime = _fadeOutTime;
	m_fFadeInTime = _fadeInTime;
}
// ================ //