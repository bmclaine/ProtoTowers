#pragma once

#include "Components\IComponent.h"

class MainMenuPrefab;

class CreditsMenuPrefab : public IComponent
{
private:
	Texture* m_pCreditsList;
	Texture* m_pSkipController;
	Texture* m_pSkipKeyboard;

	CreditsMenuPrefab& operator=(const CreditsMenuPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

protected:


public:
	CreditsMenuPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~CreditsMenuPrefab() override;

	virtual void Init() override;
	virtual void PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap) override;
	virtual void OnEnable() override;
};