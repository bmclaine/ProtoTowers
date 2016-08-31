#ifndef _BACKGROUNDMUSIC_H
#define _BACKGROUNDMUSIC_H

class IComponent;

class BackgroundMusic : public IComponent
{
private:

	int m_MusicID;
protected:
	virtual IComponent* CreateCopy(GameObject* const _gameobject, Transform* const _transform) override;
public:
	BackgroundMusic(GameObject* const _gameObject, Transform* const _transform);
	virtual ~BackgroundMusic() override;

	IComponent& operator=(IComponent&) override;
	BackgroundMusic& operator=(const BackgroundMusic& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;

};


#endif