//Author: William Zane Lewis
//
//Button that the player needs to press in order to lower the barrier

#ifndef _BARRIERBUTTON_H
#define _BARRIERBUTTON_H

#include "../Core/iInteractable.h"

class IComponent;
class GameObject;
class PlayerController;
class SpriteRenderer;

class BarrierButton : public IComponent, iInteractable
{
private:
	bool m_bIsAvailable;
	int m_iChannel;//sets which channel this button affects
	int m_iLevel;
	PlayerController* m_pInteractingPlayer;
	SpriteRenderer* m_pButtonPrompt;
	Texture* m_pPromptTexture;
	GameObject * m_pParticleEffect;
	bool m_bSpawnsMinionWave;

protected:
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;
public:
	BarrierButton(GameObject* const gameObject, Transform* const transform);
	virtual ~BarrierButton() override;

	IComponent& operator=(IComponent&) override;
	BarrierButton& operator=(const BarrierButton& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;

	virtual void OnTriggerExit(Collider * _col) override;
	virtual void OnTriggerEnter(Collider * _col) override;

	void SetIsAvailable(bool _isAvailable);
	bool GetIsAvailable();

	void SetChannel(int _channel);
	void SetParticleEffect(GameObject * _particleEffect);

	virtual void Interact() override;

};


#endif