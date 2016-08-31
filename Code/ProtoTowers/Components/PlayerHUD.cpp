#include "stdafx.h"
#include "PlayerHUD.h"
#include "Assets\Texture.h"

#include "../InfernoUI.h"
#include "Crosshair.h"
#include "MiniMap.h"
#include "ResourceCounter.h"

// ===== Constructor / Destructor ===== //
PlayerHUD::PlayerHUD(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pPlayerInfoImage = new Texture();
	m_pPlayerInfoImage->SetImage(Inferno::GetImage(L"UI_PlayerInfo_Bkgd.dds"));
	m_pHealthBarImage = new Texture();
	m_pHealthBarImage->SetImage(Inferno::GetImage(L"UI_HealthBar.dds"));
	m_pEnergyBarImage = new Texture();
	m_pEnergyBarImage->SetImage(Inferno::GetImage(L"UI_EnergyBar.dds"));
	m_pResourceImage = new Texture();
	m_pResourceImage->SetImage(Inferno::GetImage(L"UI_Resource_Bkgd.dds"));
	m_pCrosshairImage = new Texture();
	m_pCrosshairImage->SetImage(Inferno::GetImage(L"Crosshair.dds"));
	m_pBasesHealthBarImage = new Texture();
	m_pBasesHealthBarImage->SetImage(Inferno::GetImage(L"UI_BaseHealthBars_Bkgd.dds"));
	m_pLeftBaseHealthBarImage = new Texture();
	m_pLeftBaseHealthBarImage->SetImage(Inferno::GetImage(L"UI_BaseHealthBars_L.dds"));
	m_pRightBaseHealthBarImage = new Texture();
	m_pRightBaseHealthBarImage->SetImage(Inferno::GetImage(L"UI_BaseHealthBars_R.dds"));
	m_pMiniMapBorder = new Texture();
	m_pMiniMapBorder->SetImage(Inferno::GetImage(L"MiniMapBorder.dds"));
	m_pMiniMapBorderWarning = new Texture();
	m_pMiniMapBorderWarning->SetImage(Inferno::GetImage(L"MiniMapBorderWarning.dds"));
	m_pLevelUpImage = new Texture();
	m_pLevelUpImage->SetImage(Inferno::GetImage(L"UI_Weapon_Upgrade_Frame.dds"));
	m_pLevelBarImage = new Texture();
	m_pLevelBarImage->SetImage(Inferno::GetImage(L"UI_Boss_HpBar.dds"));
	m_pLevelImage = new Texture();
	m_pLevelImage->SetImage(Inferno::GetImage(L"UI_Level_Frame.dds"));
	m_pRecallBkg = new Texture();
	m_pRecallBkg->SetImage(Inferno::GetImage(L"UI_Recall_Bkg.dds"));
	m_pRecallFg = new Texture();
	m_pRecallFg->SetImage(Inferno::GetImage(L"UI_Recall_Fg.dds"));
	m_pLevelDownImage = new Texture();
	m_pLevelDownImage->SetImage(Inferno::GetImage(L"UI_Weapon_LevelDown.dds"));

	m_pMiniMap = new Material();
	m_pMiniMap->DiffuseMapID = Inferno::GetImage(L"MiniMap.dds");
	m_pMiniMap->OcclusionMapID = Inferno::GetImage(L"MiniMapMask.dds");
	m_pMiniMap->EffectID = Inferno::GetEffectByName("UI_Occlusion_Unlit");
}

PlayerHUD::~PlayerHUD()
{
	// delete Textures
	delete m_pPlayerInfoImage;
	delete m_pHealthBarImage;
	delete m_pEnergyBarImage;
	delete m_pResourceImage;
	delete m_pCrosshairImage;
	delete m_pBasesHealthBarImage;
	delete m_pLeftBaseHealthBarImage;
	delete m_pRightBaseHealthBarImage;
	delete m_pMiniMapBorder;
	delete m_pMiniMapBorderWarning;
	delete m_pLevelUpImage;
	delete m_pLevelDownImage;
	delete m_pLevelBarImage;
	delete m_pLevelImage;
	delete m_pRecallBkg;
	delete m_pRecallFg;
	
	// delete Materials
	delete m_pMiniMap;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("HUD");
}
// ==================================== //

// ===== Interface ===== //
void PlayerHUD::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{
	ObjectData* data = _dataMap.find(GetInstanceID())->second;

	std::string propName = "mapMinX";
	Property* prop = data->GetProperty(propName);
	mapMinX = (float)atof(prop->value.c_str());

	propName = "mapMaxX";
	prop = data->GetProperty(propName);
	mapMaxX = (float)atof(prop->value.c_str());

	propName = "mapMinZ";
	prop = data->GetProperty(propName);
	mapMinZ = (float)atof(prop->value.c_str());

	propName = "mapMaxZ";
	prop = data->GetProperty(propName);
	mapMaxZ = (float)atof(prop->value.c_str());
}

void PlayerHUD::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	UIRenderer* image = nullptr;
	UIImage* uiImage = nullptr;
	RectTransform* transform;

	hudCanvas = GetCanvasByName("HUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("HUD", &hudCanvas, &hudCanvasSlot);
	}

	// === Player Info Element
	UIElement* playerInfo = new UIElement();
	playerInfo->SetName("Player Info Panel");
	transform = playerInfo->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
	transform->SetPosition(174, 102);
	transform->SetBounds(Vector2(256, 128));
	//// Components
		image = playerInfo->AddComponent<UIRenderer>();
		image->SetTexture(m_pPlayerInfoImage);
		image->SetLayer(0);

	//// Children
		// == Health Bar
		UIElement* healthBar = new UIElement();
		healthBar->SetName("Player Health Bar");
		transform = healthBar->GetTransform();
		transform->SetParent(playerInfo->GetTransform());
		transform->SetPosition(43.0f, -12.0f);
		transform->SetBounds(Vector2(126, 16));
		//// Components
			image = healthBar->AddComponent<UIRenderer>();
			image->SetTexture(m_pHealthBarImage);
			image->SetEffect(UI_FILL_EFFECT);
			image->SetImageFill(1);
			image->SetColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
			image->SetLayer(2);

		// == Energy Bar
		UIElement* energyBar = new UIElement();
		energyBar->SetName("Player Energy Bar");
		transform = energyBar->GetTransform();
		transform->SetParent(playerInfo->GetTransform());
		transform->SetPosition(35.0f, 8);
		transform->SetBounds(Vector2(110, 14));
		//// Components
			image = energyBar->AddComponent<UIRenderer>();
			image->SetTexture(m_pEnergyBarImage);
			image->SetEffect(UI_FILL_EFFECT);
			image->SetImageFill(1);
			image->SetColor(Color(1.0f, 0.55f, 0.0f, 1.0f));
			image->SetLayer(3);
		//// Children
			// == Energy Bar Overheat Indicator
			UIElement* energyOverheatIndicator = new UIElement();
			energyOverheatIndicator->SetName("Overheat Indicator");
			transform = energyOverheatIndicator->GetTransform();
			transform->SetParent(energyBar->GetTransform());
			transform->SetBounds(Vector2(110, 15));
			//// Components
				image = energyOverheatIndicator->AddComponent<UIRenderer>();
				image->SetTexture(m_pEnergyBarImage);
				image->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
				image->SetLayer(2);

	AddToView(playerInfo, "HUD");

	// === Resources Element
	UIElement* resourcesPanel = new UIElement();
	resourcesPanel->SetName("Resources Panel");
	transform = resourcesPanel->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPRIGHT);
	transform->SetPosition(-147, 102);
	transform->SetBounds(Vector2(256, 128));
	//// Components
		// == UIRenderer
		image = resourcesPanel->AddComponent<UIRenderer>();
		image->SetTexture(m_pResourceImage);
		image->SetLayer(0);
	
	//// Children
		// === Resources Text
		UIElement* resourcesText = new UIElement();
		resourcesText->SetName("Resources Counter");
		transform = resourcesText->GetTransform();
		transform->SetParent(resourcesPanel->GetTransform());
		transform->SetPosition(-35.0f, 0.0f);
		transform->SetBounds(Vector2(80.0f, 30.0f));
		//// Components
			// == UIText
			UIText* rTxt = resourcesText->AddComponent<UIText>();
			rTxt->SetText("0000");
			rTxt->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
			rTxt->SetFontSize(22.0f);
			rTxt->SetLayer(2);
			rTxt->SetAlignment(TextAlignment::CENTER);
			// == Resource Counter
			resourcesText->AddComponent<ResourceCounter>();

	AddToView(resourcesPanel, "HUD");

	// === Crosshair
	UIElement* crosshair = new UIElement();
	crosshair->SetName("Crosshair");
	transform = crosshair->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_CENTER);
	transform->SetPosition(0.0f, 32.0f);
	transform->SetBounds(Vector2(64, 64));
	//// Components
		// === Crosshair Image
		image = crosshair->AddComponent<UIRenderer>();
		image->SetTexture(m_pCrosshairImage);
		// === Crosshair Script
		crosshair->AddComponent<Crosshair>();
	

	AddToView(crosshair, "HUD");

	// === Base Health Bars (Bkgd)
	UIElement* baseHealthBkgd = new UIElement();
	baseHealthBkgd->SetName("Base HealthBars Panel");
	image = baseHealthBkgd->AddComponent<UIRenderer>();
	image->SetTexture(m_pBasesHealthBarImage);
	image->SetColor(Color(0.5f, 0.5f, 0.5f, 0.5f));
	image->SetLayer(0);
	transform = baseHealthBkgd->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetPosition(0.0f, 60.0f);
	transform->SetBounds(Vector2(416, 32));

	// === Left Base Health (Friendly Base)
	UIElement* leftBaseHealth = new UIElement();
	leftBaseHealth->SetName("Left Base HealthBar");
	image = leftBaseHealth->AddComponent<UIRenderer>();
	image->SetTexture(m_pLeftBaseHealthBarImage);
	image->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
	image->SetLayer(1);
	image->SetEffect(UI_FILL_RIGHT_EFFECT);
	image->SetImageFill(1.0f);
	transform = leftBaseHealth->GetTransform();
	transform->SetParent(baseHealthBkgd->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_CENTER);
	transform->SetPosition(-104.0f, 0.0f);
	transform->SetBounds(Vector2(205.0f, 30.0f));

	// === Right Base Health (Enemy Base)
	UIElement* rightBaseHealth = new UIElement();
	rightBaseHealth->SetName("Right Base HealthBar");
	image = rightBaseHealth->AddComponent<UIRenderer>();
	image->SetTexture(m_pRightBaseHealthBarImage);
	image->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	image->SetLayer(1);
	image->SetEffect(UI_FILL_EFFECT);
	image->SetImageFill(1.0f);
	transform = rightBaseHealth->GetTransform();
	transform->SetParent(baseHealthBkgd->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_CENTER);
	transform->SetPosition(104.0f, 0.0f);
	transform->SetBounds(Vector2(205.0f, 30.0f));

	AddToView(baseHealthBkgd, "HUD");

	// === MiniMap
	UIElement* miniMap = new UIElement();
	miniMap->GetTransform()->SetAnchors(Anchors::ANCHOR_BOTTOMRIGHT);
	miniMap->GetTransform()->SetPosition(Vector2(-90.0f, -90.0f));
	miniMap->GetTransform()->SetBounds(Vector2(180.0f, 180.0f));
	//miniMap->GetTransform()->SetScale(Vector2(300.0f, 300.0f));
	miniMap->SetName("MiniMap");
	uiImage = miniMap->AddComponent<UIImage>();
	uiImage->SetMaterial(m_pMiniMap);
	uiImage->SetLayer(1);
	// === MiniMapBorder
	UIElement* miniMapBorder = new UIElement();
	miniMapBorder->GetTransform()->SetBounds(Vector2(209.0f, 209.0f));
	miniMapBorder->GetTransform()->SetParent(miniMap->GetTransform());
	miniMapBorder->SetName("MiniMapBorder");
	image = miniMapBorder->AddComponent<UIRenderer>();
	image->SetTexture(m_pMiniMapBorder);
	image->SetLayer(0);
	// === MiniMapWarningBorder
	UIElement* miniMapBorderWarning = new UIElement();
	miniMapBorderWarning->GetTransform()->SetBounds(Vector2(209.0f, 209.0f));
	miniMapBorderWarning->GetTransform()->SetParent(miniMap->GetTransform());
	miniMapBorderWarning->SetName("MiniMapBorderWarning");
	image = miniMapBorderWarning->AddComponent<UIRenderer>();
	image->SetTexture(m_pMiniMapBorderWarning);
	image->SetLayer(1);

	MiniMap* mapScript = miniMap->AddComponent<MiniMap>();
	mapScript->SetWorldBounds(mapMinX, mapMaxX, mapMinZ, mapMaxZ);

	AddToView(miniMap, "HUD");

	UIElement* recallBkg = new UIElement();
	recallBkg->SetName("Recall BKG");
	image = recallBkg->AddComponent<UIRenderer>();
	image->SetTexture(m_pRecallBkg);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	image->SetLayer(1);
	image->SetEffect(UIEffects::UI_UNLIT_EFFECT);
	transform = recallBkg->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMLEFT);
	transform->SetPosition(95.0f, -120.0f);
	transform->SetBounds(Vector2(128.0f, 20.0f));

	UIElement* recallFg = new UIElement();
	recallFg->SetName("Recall Bar");
	image = recallFg->AddComponent<UIRenderer>();
	image->SetTexture(m_pRecallFg);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	image->SetLayer(2);
	image->SetImageFill(1.0f);
	image->SetEffect(UIEffects::UI_FILL_EFFECT);
	transform = recallFg->GetTransform();
	transform->SetPosition(0.0f, 0.0f);
	transform->SetParent(recallBkg->GetTransform());
	transform->SetBounds(Vector2(127.0f, 18.0f));

	AddToView(recallBkg, "HUD");

	UIElement* levelUpUI = new UIElement();
	levelUpUI->SetName("LevelUp");
	image = levelUpUI->AddComponent<UIRenderer>();
	image->SetTexture(m_pLevelUpImage);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	image->SetLayer(1);
	image->SetEffect(UIEffects::UI_UNLIT_EFFECT);
	image->SetImageFill(1.0f);
	transform = levelUpUI->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetPosition(0.0f, -64.0f);
	transform->SetBounds(Vector2(256.0f, 128.0f));

	UIElement* levelUpText = new UIElement();
	levelUpText->GetTransform()->SetParent(levelUpUI->GetTransform());
	levelUpText->SetName("LevelUpText");
	levelUpText->GetTransform()->SetAnchors(Anchors::ANCHOR_CENTER);
	UIText* levelText = levelUpText->AddComponent<UIText>();
	levelText->SetText("Level Up");
	levelText->SetFontSize(20);
	levelText->SetAlignment(TextAlignment::CENTER);
	levelText->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
	levelText->SetLayer(2);

	UIElement* levelDownUI = new UIElement();
	levelDownUI->SetName("LevelDown");
	image = levelDownUI->AddComponent<UIRenderer>();
	image->SetTexture(m_pLevelDownImage);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	image->SetLayer(1);
	image->SetEffect(UIEffects::UI_UNLIT_EFFECT);
	image->SetImageFill(1.0f);
	transform = levelDownUI->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetPosition(0.0f, -64.0f);
	transform->SetBounds(Vector2(256.0f, 128.0f));

	UIElement* levelDownText = new UIElement();
	levelDownText->GetTransform()->SetParent(levelDownUI->GetTransform());
	levelDownText->SetName("LevelDownText");
	levelDownText->GetTransform()->SetAnchors(Anchors::ANCHOR_CENTER);
	levelText = levelDownText->AddComponent<UIText>();
	levelText->SetText("Level Down");
	levelText->SetFontSize(20);
	levelText->SetAlignment(TextAlignment::CENTER);
	levelText->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
	levelText->SetLayer(2);

	AddToView(levelDownUI, "HUD");

	UIElement* levelUpBarBkg = new UIElement();
	levelUpBarBkg->SetName("LevelUpBarBkg");
	image = levelUpBarBkg->AddComponent<UIRenderer>();
	image->SetTexture(m_pLevelBarImage);
	image->SetLayer(1);
	image->SetEffect(UIEffects::UI_UNLIT_EFFECT);
	image->SetColor(Color(0.5f, 0.5f, 0.5f, 1.0f));
	levelUpBarBkg->GetTransform()->SetAnchors(Anchors::ANCHOR_TOPLEFT);
	levelUpBarBkg->GetTransform()->SetPosition(32.0f, 90.0f);
	levelUpBarBkg->GetTransform()->SetBounds(Vector2(16.0f, 90.0f));

	UIElement* levelUpBar = new UIElement();
	levelUpBar->SetName("LevelUpBar");
	image = levelUpBar->AddComponent<UIRenderer>();
	image->SetTexture(m_pLevelBarImage);
	image->SetLayer(2);
	image->SetEffect(UIEffects::UI_FILL_TOP_EFFECT);
	image->SetColor(Color(0.0f, 0.3f, 1.0f, 1.0f));
	image->SetImageFill(1.0f);
	levelUpBar->GetTransform()->SetParent(levelUpBarBkg->GetTransform());
	levelUpBar->GetTransform()->SetBounds(Vector2(16.0f, 90.0f));

	UIElement* levelIndicatorUI = new UIElement();
	levelIndicatorUI->SetName("LevelIndicator");
	image = levelIndicatorUI->AddComponent<UIRenderer>();
	image->SetTexture(m_pLevelImage);
	image->SetLayer(1);
	image->SetEffect(UIEffects::UI_UNLIT_EFFECT);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	transform = levelIndicatorUI->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
	transform->SetPosition(32.0f, 160.0f);
	transform->SetBounds(Vector2(45.0f, 45.0f));

	UIElement* levelIndicatorText = new UIElement();
	levelIndicatorText->GetTransform()->SetParent(levelIndicatorUI->GetTransform());
	levelIndicatorText->SetName("LevelText");
	levelIndicatorText->GetTransform()->SetAnchors(Anchors::ANCHOR_CENTER);
	UIText* lIndicatorText = levelIndicatorText->AddComponent<UIText>();
	lIndicatorText->SetText("1");
	lIndicatorText->SetFontSize(20);
	lIndicatorText->SetAlignment(TextAlignment::CENTER);
	lIndicatorText->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
	lIndicatorText->SetLayer(2);

	AddToView(levelUpBarBkg, "HUD");

	AddToView(levelIndicatorUI, "HUD");

	AddToView(levelUpUI, "HUD");
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& PlayerHUD::operator=(IComponent& _rhs)
{
	return *this;
}

PlayerHUD& PlayerHUD::operator=(const PlayerHUD& _rhs)
{
	return *this;
}

IComponent* PlayerHUD::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new PlayerHUD(gameObject, transform);
}
// =============================== //