#pragma once
#include "Components\IComponent.h"

class ToolTipPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pBackgroundTexture;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	ToolTipPrefab& operator=(const ToolTipPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	ToolTipPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~ToolTipPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

