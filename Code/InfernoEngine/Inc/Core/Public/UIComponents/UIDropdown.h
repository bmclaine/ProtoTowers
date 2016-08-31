#pragma once

#include "UISelectable.h"
#include "UIText.h"
#include "InputEvents.h"

#include <vector>

class Texture;
class UIText;
class UIRenderer;

typedef std::function<void(void)> SelectionFunction;

class UIDropdown : public UISelectable
{
private:
	std::vector<std::wstring> m_vValues;
	Vector4 m_f4SelectionRegion;
	std::wstring m_wsStartText;

	SelectionFunction m_pFunciton;

	Texture* m_pDownArrowUnselected;
	Texture* m_pDownArrowHovered;
	Texture* m_pDownArrowSelected;
	Texture* m_pDropdownBox;
	Texture* m_pSelectedFieldBackground;
	Texture* m_pSelectionFieldForeground;
	Texture* m_pSelectionFieldBackground;

	UIRenderer* m_pArrowRenderer;
	UIRenderer* m_pBoxRenderer;
	UIElement* m_pSelectionElement;
	UIElement* m_pSelectionBoxBackground;
	UIElement* m_pSelectionBoxForeground;
	UIText* m_pSelectedText;
	UIText* m_pSelectionText;

	POINT m_i2MousePos;
	UINT m_uiLayer;
	float m_fFontSize;
	TextAlignment m_taAlignment;
	float m_fCellSize;
	int m_iSelectedElement;
	int m_iSelectionElement;

	bool m_bIsSelected;

	void MoveSelectionUp();
	void MoveSelectionDown();

public:
	UIDropdown(UIElement* _owner);
	virtual ~UIDropdown() override;

	virtual void OnSelect() override;
	void OnPressed();
	void OnExit(ACTION_STATUS status);
	virtual void OnDeselect() override;
	void OnMouseHover(POINT _cursorPosition, POINT _displacement);
	void OnResolutionChange(void* pData);

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

	// === Init
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called to initialize the component. Can be overrriden by any derived UIComponents.
	virtual void Init();
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
	std::wstring GetSelectedValue() const;
	int GetSelectionID() const;

	void SetFunctionToCallOnSelectionChange(SelectionFunction function);
	void SetStartingValue(std::wstring value);
	void SetStartingValue(UINT index);
	void SetSelectedValue(std::wstring value);
	void SetSelectedValue(UINT index);
	void SetSelectableValues(std::vector<std::wstring>& values);
	void SetLayer(UINT layer);
	void SetFontSize(float fontSize);
	void SetFontAlignment(TextAlignment alignment);
};

