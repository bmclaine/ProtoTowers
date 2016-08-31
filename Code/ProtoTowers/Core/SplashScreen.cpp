#include "stdafx.h"
#include "SplashScreen.h"
#include "InfernoUI.h"

#include "ModuleInterface.h"

#define FADE_TIME 0.25f
#define SPLASH_TIME 2.5f

SplashScreen::SplashScreen()
{
	m_pGPSplash = new Texture();
	m_pGPSplash->SetImage(InfernoEngine_Graphics::LoadTexture(L"UI_GPGames_Splash.dds"));
	m_pStudioSplash = new Texture();
	m_pStudioSplash->SetImage(InfernoEngine_Graphics::LoadTexture(L"UI_Sakaruba_Splash.dds"));
	m_pTeamSplash = new Texture();
	m_pTeamSplash->SetImage(InfernoEngine_Graphics::LoadTexture(L"UI_SpaceYetis_Splash.dds"));

	m_bSplashesFinished = false;
	m_iSplashID = 0;

	GameObject* cameraObject = Inferno::Instantiate();
	Camera* camera = cameraObject->AddComponent<Camera>();
	camera->SetClearType(ClearType::CLEAR_TO_COLOR);
	camera->SetClearColor(Color(0, 0, 0, 1));
	CameraManager::SetMainCamera(camera);

	UICanvas* canvas = nullptr;
	UINT canvasID = -1;

	Inferno_UI::CreateNewCanvas("Splash", &canvas, &canvasID);

	UIElement* SplashElement = new UIElement();
	RectTransform* trasnform = SplashElement->GetTransform();
	trasnform->SetBounds(Vector2(1024, 768));

	m_pSplashRenderer = SplashElement->AddComponent<UIRenderer>();
	m_pSplashRenderer->SetTexture(m_pGPSplash);
	m_pSplashRenderer->SetColor(Color(0, 0, 0, 1));

	Inferno_UI::AddToView(SplashElement, canvasID);

	m_itFadeInTimer.SetOnTimerEndFunction(std::bind(&SplashScreen::OnFadeInFinish, this));
	m_itSplashTimer.SetOnTimerEndFunction(std::bind(&SplashScreen::OnSplashFinish, this));
	m_itFadeOutTimer.SetOnTimerEndFunction(std::bind(&SplashScreen::OnFadeOutFinish, this));

	m_itFadeInTimer.SetTime(FADE_TIME);
}

SplashScreen::~SplashScreen()
{
	SAFE_DELETE(m_pGPSplash);
	SAFE_DELETE(m_pStudioSplash);
	SAFE_DELETE(m_pTeamSplash);
}

void SplashScreen::Update()
{
	if (!m_bSplashesFinished)
	{
		if (!m_itFadeInTimer.IsFinished())
		{
			float color = 1.0f - (m_itFadeInTimer.TimeRemaining() / m_itFadeInTimer.GetSetTimeSeconds());
			m_pSplashRenderer->SetColor(Color(color, color, color, 1));
		}
		else if (!m_itFadeOutTimer.IsFinished())
		{
			float color = m_itFadeOutTimer.TimeRemaining() / m_itFadeOutTimer.GetSetTimeSeconds();
			m_pSplashRenderer->SetColor(Color(color, color, color, 1));
		}
		else
			m_pSplashRenderer->SetColor(Color(1, 1, 1, 1));
	}
}

// Event Functions
void SplashScreen::OnFadeInFinish()
{
	m_itSplashTimer.SetTime(SPLASH_TIME);
}
void SplashScreen::OnSplashFinish()
{
	m_itFadeOutTimer.SetTime(FADE_TIME);
}
void SplashScreen::OnFadeOutFinish()
{
	if (m_iSplashID == 0)
	{
		m_pSplashRenderer->SetTexture(m_pStudioSplash);
		m_itFadeInTimer.SetTime(FADE_TIME);
		m_iSplashID++;
	}
	else if (m_iSplashID == 1)
	{
		m_pSplashRenderer->SetTexture(m_pTeamSplash);
		m_itFadeInTimer.SetTime(FADE_TIME);
		m_iSplashID++;
	}
	else
	{
		m_bSplashesFinished = true;
	}
}

// Getters
bool SplashScreen::IsFinished()
{
	return m_bSplashesFinished;
}