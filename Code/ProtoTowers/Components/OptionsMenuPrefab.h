#pragma once

#include "Components\IComponent.h"

class UICanvas;
class MainMenuPrefab;

class OptionsMenuPrefab : public IComponent
{
private:
	Texture* m_pBtnImageN;
	Texture* m_pBtnImageS;
	Texture* m_pDpDwnImageN;
	Texture* m_pDpDwnImageH;
	Texture* m_pDpDwnImageS;
	Texture* m_pResolutionText;
	Texture* m_pWindowText;
	Texture* m_pApplyButtonText;
	Texture* m_pBackButtonText;
	Texture* m_pDefaultButtonText;
	Texture* m_pOKButtonText;
	Texture* m_pCancelButtonText;
	Texture* m_pOptionsText;
	Texture* m_pGraphicsText;
	Texture* m_pAudioText;
	Texture* m_pOptionsTextBackground;
	Texture* m_pOptionsMenuBackground;
	Texture* m_pOptionsAcceptBackground;

	std::string m_sPrevCanvas;

	OptionsMenuPrefab& operator=(const OptionsMenuPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	OptionsMenuPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~OptionsMenuPrefab() override;

	virtual void Init() override;
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;

	void SetPreviousCanvas(std::string name);
};

