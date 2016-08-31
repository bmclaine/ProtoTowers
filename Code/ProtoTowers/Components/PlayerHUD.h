#pragma once
#include "Components\IComponent.h"

class PlayerHUD : public IComponent
{
private:
	// === Variables
	Texture* m_pPlayerInfoImage;
	Texture* m_pHealthBarImage;
	Texture* m_pEnergyBarImage;
	Texture* m_pWeaponIcon;
	Texture* m_pResourceImage;
	Texture* m_pResourceIcon;
	Texture* m_pCrosshairImage;
	Texture* m_pBasesHealthBarImage;
	Texture* m_pLeftBaseHealthBarImage;
	Texture* m_pRightBaseHealthBarImage;
	Texture* m_pMiniMapBorder;
	Texture* m_pMiniMapBorderWarning;
	Texture* m_pLevelUpImage;
	Texture* m_pLevelDownImage;
	Texture* m_pLevelBarImage;
	Texture* m_pLevelImage;
	Texture* m_pRecallBkg;
	Texture* m_pRecallFg;

	Material* m_pMiniMap;

	float mapMinX, mapMaxX;
	float mapMinZ, mapMaxZ;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PlayerHUD& operator=(const PlayerHUD& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PlayerHUD(GameObject* _gameObject, Transform* _transform);
	virtual ~PlayerHUD();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

