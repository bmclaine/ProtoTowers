#pragma once
#include "Components\IComponent.h"

class WinHUDPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pWinImage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	WinHUDPrefab& operator=(const WinHUDPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	WinHUDPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~WinHUDPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

