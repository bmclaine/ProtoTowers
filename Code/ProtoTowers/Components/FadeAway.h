#pragma once
#include "UIComponents\UIComponent.h"

class UIRenderer;
struct Color;

class FadeAway : public UIComponent
{
private:
	// === Variables
	Color* m_pColor;
	Texture* m_pFadeTexture;
	UIRenderer* m_pImage;
	float m_fFadeOutTime, m_fFadeInTime;
	float m_fFadeTimer;
	bool m_bFadingIn;

public:
	// === Constructor / Destructor === //
	FadeAway(UIElement* _owner);
	virtual ~FadeAway();
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //

	// === Accessors === //
	Color* GetColor() const;
	// ================= //

	// === Mutators === //
	void SetFadeColor(float _r, float _g, float _b, float _a);
	void SetFadeTime(float _fadeOutTime, float _fadeInTime = 0.0f);
	// ================ //
};

