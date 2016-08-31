#pragma once

#include "UISelectable.h"
#include "Color.h"

class Texture;
class UIRenderer;

typedef std::function<void(void*)> FillChangeFunction;

class UISlider : public UISelectable
{
private:
	Color m_cBackgroundColor;
	Color m_cForegroundColor;
	Color m_cSelectionRegionHoveredColor;
	Color m_cSelectionRegionUnselectedColor;
	Color m_cSelectionRegionSelectedColor;

	Texture* m_pBackgroundImage;
	Texture* m_pForegroundImage;
	Texture* m_pSelectionRegionImage;

	UIRenderer* m_pFillImageOne;
	UIRenderer* m_pFillImageTwo;
	UIRenderer* m_pSelectionRegion;

	FillChangeFunction m_pFunc;

	UINT m_uiLayer;
	float m_fFillAmount;
	bool m_bSelected;
	bool m_bFillRight;

	void MoveSliderRight();
	void MoveSliderLeft();

public:
	UISlider(UIElement* _owner);
	virtual ~UISlider() override;

	virtual void OnSelect() override;
	void OnPressed();
	void OnRelease();
	virtual void OnDeselect() override;
	void OnMouseHover(POINT _cursorPosition, POINT _displacement);

	// === OnEnabled
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
	void OnEnabled();
	// === OnDisabled
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is disabled
	void OnDisabled();

	// === PostInit
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
	virtual void PostInit();
	// === Update
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called every frame. Can be overriden by any derived UIComponents.
	virtual void Update();

	virtual UISelectable* GetUpSelectable() const override;
	virtual UISelectable* GetDownSelectable() const override;
	virtual UISelectable* GetRightSelectable() const override;
	virtual UISelectable* GetLeftSelectable() const override;
	float GetFill() const;

	void SetOnFillChangeFunction(FillChangeFunction func);
	void SetBackImageColor(Color color);
	void SetFillImageColor(Color color);
	void SetUnselectedColor(Color color);
	void SetHoveredColor(Color color);
	void SetSelectedColor(Color color);
	void SetBackImage(Texture* image);
	void SetFrontImage(Texture* image);
	void SetSelectedImage(Texture* image);
	void SetFill(float fill);
	void SetLayer(UINT layer);
	void SetFillRight(bool fillRight);
};

