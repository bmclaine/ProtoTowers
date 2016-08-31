#include "stdafx.h"
#include "EndGame.h"
#include "../Public/ModuleInterface.h"

// ===== Constructor / Destructor ===== //
EndGame::EndGame(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pEndGameTimer = new InfernoTimer(4.0f);
	m_eWinningTeam = Team::NO_TEAM;
}

EndGame::~EndGame()
{
	delete m_pEndGameTimer;
}
// ==================================== //

// ===== Interface ===== //
void EndGame::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void EndGame::Update()
{
	if (m_pEndGameTimer->IsFinished()) {
		if (!m_sSceneName.empty()) {
			InfernoEngine_SceneManagement::LoadScene(m_sSceneName.c_str());
		}
		else if (m_eWinningTeam == Team::BLUE) {
			InfernoEngine_SceneManagement::LoadScene("WinScene");
		}
		else {
			InfernoEngine_SceneManagement::LoadScene("LoseScene");
		}
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& EndGame::operator=(IComponent& _rhs)
{
	EndGame* rhsEndGame = dynamic_cast<EndGame*>(&_rhs);
	if (rhsEndGame == nullptr) {
		return *this;
	}

	return operator=(*rhsEndGame);
}

EndGame& EndGame::operator=(const EndGame& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_eWinningTeam = _rhs.m_eWinningTeam;
		*m_pEndGameTimer = *_rhs.m_pEndGameTimer;
	}

	return *this;
}

IComponent* EndGame::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	EndGame* newEndGame = new EndGame(_gameObject, _transform);
	*newEndGame = *this;

	return newEndGame;
}
// =============================== //

// ===== Mutators ===== //
void EndGame::SetDelayTime(float _seconds)
{
	m_pEndGameTimer->SetTime(_seconds);
}

void EndGame::SetWinningTeam(Team _winningTeam)
{
	m_eWinningTeam = _winningTeam;
}

void EndGame::SetSceneToLoad(std::string _sceneName)
{
	m_sSceneName = _sceneName;
}
// ==================== //
