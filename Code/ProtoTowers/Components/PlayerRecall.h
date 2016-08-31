#pragma once

#include "Components\IComponent.h"

class ParticleSystem;
class FadeAway;

class PlayerRecall : public IComponent{

private:
	FadeAway* m_pFadeAway;
	bool m_bRecalling;
	float m_fRecallTimer;

protected:
	PlayerRecall& operator=(const PlayerRecall& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform);

public:
	PlayerRecall(GameObject* const gameObject, Transform* const transform);

	virtual ~PlayerRecall() override;

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable() override;

	virtual void OnDisable() override;

	virtual void OnDestroy() override;

	virtual void Update() override;

	void SetRecalling(bool isRecalling);
	bool GetRecalling() const;

	void SetRecallTimer(float recallTimer);

	void StartRecall();
	void StopRecall();
};