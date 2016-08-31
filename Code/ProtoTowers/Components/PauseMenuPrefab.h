#pragma once
#include "Components\IComponent.h"

class Texture;

class PauseMenuPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pPauseMenuImage;
	Texture* m_pBtnImageN;
	Texture* m_pBtnImageS;
	Texture* m_pPMResumeImage, *m_pOptionTxtImage, *m_pPMHowToImage, *m_pPMQuitImage;
	Texture* m_pControlsInfoImage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PauseMenuPrefab& operator=(const PauseMenuPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PauseMenuPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~PauseMenuPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

