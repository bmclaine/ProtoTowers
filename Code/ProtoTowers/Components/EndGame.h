// Author : Jack Gillette
//
// Handles Ending the Game after a certain amount of time

#pragma once
#include "Components\IComponent.h"

#include "Entity.h"

class InfernoTimer;

class EndGame : public IComponent
{
private:
	// === Variables
	InfernoTimer* m_pEndGameTimer;
	std::string m_sSceneName;
	Team m_eWinningTeam;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	EndGame& operator=(const EndGame& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	EndGame(GameObject* _gameObject, Transform* _transform);
	virtual ~EndGame();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void Update() override;
	// ================= //

	// === Mutators === //
	void SetDelayTime(float _seconds);
	void SetWinningTeam(Team _winningTeam);
	void SetSceneToLoad(std::string _sceneName);
	// ================ //
};

