#pragma once
#include "Components\IComponent.h"

class OptionsMenuPrefab;

class MainMenuPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pTitleImage;
	Texture* m_pBtnImageN;
	Texture* m_pBtnImageS;
	Texture* m_pPlayTxtImage;
	Texture* m_pOptionTxtImage;
	Texture* m_pCreditsTxtImage;
	Texture* m_pQuitTxtImage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MainMenuPrefab& operator=(const MainMenuPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	MainMenuPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~MainMenuPrefab();
	// ================================ //

	// === Interface === //
	virtual void Init() override;
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

