#include "stdafx.h"
#include "ControlPointProgressBarPrefab.h"
#include "../InfernoUI.h"
#include "ControlPointProgressBar.h"


ControlPointProgressBarPrefab::ControlPointProgressBarPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pProgressBarTexture = new Texture();
	m_pProgressBarTexture->SetImage(Inferno::GetImage(L"Tower_HP_Bar.dds"));
}

ControlPointProgressBarPrefab::~ControlPointProgressBarPrefab()
{
	delete m_pProgressBarTexture;

	Inferno_UI::DestroyCanvas("ProgressBarHUD");
}

IComponent& ControlPointProgressBarPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

ControlPointProgressBarPrefab& ControlPointProgressBarPrefab::operator=(const ControlPointProgressBarPrefab& _rhs)
{
	return *this;
}

IComponent* ControlPointProgressBarPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new ControlPointProgressBarPrefab(gameObject, transform);
}


void ControlPointProgressBarPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void ControlPointProgressBarPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform;

	hudCanvas = GetCanvasByName("ProgressBarHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ProgressBarHUD", &hudCanvas, &hudCanvasSlot);
	}



	UIElement* progressBarHUD = new UIElement();
	progressBarHUD->SetName("progress bar HUD");
	transform = progressBarHUD->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetPosition(Vector2(0, 128));
	transform->SetBounds(Vector2(300, 50));


	UIRenderer* image = progressBarHUD->AddComponent<UIRenderer>();
	image->SetTexture(m_pProgressBarTexture);
	image->SetEffect(UI_FILL_EFFECT);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	image->SetLayer(1);

	UIElement* progressBarHUDBackground = new UIElement();
	progressBarHUDBackground->SetName("progress bar HUD Background");
	transform = progressBarHUDBackground->GetTransform();
	transform->SetParent(progressBarHUD->GetTransform());
	transform->SetBounds(Vector2(300, 50));


	UIRenderer* imageBackground = progressBarHUDBackground->AddComponent<UIRenderer>();
	imageBackground->SetTexture(m_pProgressBarTexture);
	imageBackground->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	imageBackground->SetLayer(0);
	imageBackground->SetEnabled(false);
	
	ControlPointProgressBar * cpProgressBar = progressBarHUD->AddComponent<ControlPointProgressBar>();
	cpProgressBar->SetProgressBar(image);
	cpProgressBar->SetProgressBarBackground(imageBackground);
	image->SetEnabled(false);
	


	
	AddToView(progressBarHUD, "ProgressBarHUD");
	AddToView(progressBarHUDBackground, "progressBarHUDBackground");
}
