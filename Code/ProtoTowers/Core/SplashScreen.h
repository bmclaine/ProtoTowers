#pragma once

#include "InfernoTimer.h"

class Texture;
class UIRenderer;

class SplashScreen
{
private:
	InfernoTimer m_itFadeInTimer;
	InfernoTimer m_itSplashTimer;
	InfernoTimer m_itFadeOutTimer;
	Texture* m_pGPSplash;
	Texture* m_pStudioSplash;
	Texture* m_pTeamSplash;
	UIRenderer* m_pSplashRenderer;
	int m_iSplashID;
	bool m_bSplashesFinished;

public:
	SplashScreen();
	~SplashScreen();

	void Update();

	// Event Functions
	void OnFadeInFinish();
	void OnSplashFinish();
	void OnFadeOutFinish();

	// Getters
	bool IsFinished();
};

