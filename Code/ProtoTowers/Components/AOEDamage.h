#pragma once
#include "Components\IComponent.h"

class AOEDamage : public IComponent{

private:
	float m_fDamage;
	float m_fRange;

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	AOEDamage& operator=(const AOEDamage& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

public:
	AOEDamage(GameObject* _gameObject, Transform* _transform);
	virtual ~AOEDamage();

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	void Cast();
};