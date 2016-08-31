// Author : Jack Gillette
//
// Handles displaying the Names of Towers and their Prices over top of the Tower's Button in the Menu
#pragma once
#include "UIComponents\UIComponent.h"

class UIText;
enum TowerTypes;

class TowerInfo : public UIComponent
{
private:
	// === Enumerations
	enum TowerInfoFlags { TI_CanBuy_Flag = 0, TI_Unavailable_Flag = 1, TI_MAX_FLAGS };

	// === Variables
	UIRenderer* m_pUnavailableImg;
	UIText* m_pName;
	UIText* m_pPrice;
	Texture* m_pUnavailableTexture;
	TowerTypes m_eType;
	float m_fNameXOffset, m_fNameYOffset;
	float m_fUnavailXOffset, m_fUnavailYOffset;
	float m_fPriceXOffset, m_fPriceYOffset;
	unsigned int m_uiTowerCost;
	Flags<char> m_fcTowerInfoFlags;

	// === Private Interface === //
	void OnResourceUpdate(unsigned int _newAmount);
	// ========================= //

public:
	// === Constructor / Destructor === //
	TowerInfo(UIElement* _owner);
	virtual ~TowerInfo();
	// ================================ // 

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	// ================= //

	// === Mutators === //
	void SetType(TowerTypes _type);
	void SetNameOffset(float _xOffset, float _yOffset);
	void SetUnavailableOffset(float _xOffset, float _yOffset);
	void SetPriceOffset(float _xOffset, float _yOffset);
	void SetAsUnavailable(bool _value);
	// ================ //
};

