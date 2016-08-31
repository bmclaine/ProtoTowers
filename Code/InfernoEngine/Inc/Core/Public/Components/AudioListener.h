#pragma once

#include "Components\IComponent.h"

class AudioListener : public IComponent{

private:
	bool m_bRegistered;

protected:
	AudioListener& operator=(const AudioListener& _rhs);
	virtual IComponent& operator=(IComponent& _rhs) override;
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform);
	~AudioListener() = default;

public:
	AudioListener(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable() override;
};