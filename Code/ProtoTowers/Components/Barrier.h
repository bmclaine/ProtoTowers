//Author: Zane Lewis
//
//A barrier that kills friendly minions and prevents the player from rushing the enemy base 
#pragma once

#define INACTIVE_OFFSET -20

class IComponent;
class Collider;
class UIElement;
class UIRenderer;
class Texture;
struct Material;

class Barrier : public IComponent
{
private:
	struct MapElement
	{
		Transform* transform;
		UIElement* element;
	};

	UIElement  *m_pIconElement;
	UIRenderer *m_pIconRenderer;
	Texture    *m_pIcon;
	MapElement  m_pMapElement;

	bool m_bIsActive;//if the barrier is active or not
	float m_fDistanceMoved;//the distance that the barrier has moved so far
	float m_fVelocity;//the speed at which it disables and enables
	int m_iChannel;//Channel which determines what button affects which barrier
	Material* m_pMaterial;//Material used by the barrier

	//DESCRIPTION: Switches the barrier from deactivated state to activated state
	//
	//IN: void
	//
	//OUT: void
	void Activate();

	//DESCRIPTION: Switches the barrier from activated to deactivated state
	//
	//IN: void
	//
	//OUT: void
	void Deactivate();
protected:
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;
public:
	Barrier(GameObject* const _gameObject, Transform* const _transform);

	virtual ~Barrier()override;

	IComponent& operator=(IComponent&) override;
	Barrier& operator=(const Barrier& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;

	virtual void OnCollisionEnter(Collider * _col) override;

	//DESCRIPTION: sets the state of the barrier
	//
	//IN: bool - boolean to be set
	//
	//OUT: void
	void SetActiveFalse(void* _data);

	bool GetActive();

	void SetChannel(int _channel);
};
