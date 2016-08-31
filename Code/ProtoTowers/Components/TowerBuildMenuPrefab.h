#pragma once
#include "Components\IComponent.h"

class Texture;

class TowerBuildMenuPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pBuildMenuImage;
	Texture* m_pBMLeftBtnImage;
	Texture* m_pBMRightBtnImage;
	Texture* m_pAT_Icon, *m_pLT_Icon, *m_pFT_Icon, *m_pM_Icon, *m_pPT_Icon;
	Texture* m_pAT_W_Icon, *m_pLT_W_Icon, *m_pFT_W_Icon, *m_pM_W_Icon, *m_pPT_W_Icon;
	Texture* m_pResourceIcon;
	Texture* m_pControlsInfoImage;
	Texture* m_pTowerLabelImage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	TowerBuildMenuPrefab& operator=(const TowerBuildMenuPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	TowerBuildMenuPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~TowerBuildMenuPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

