#include "stdafx.h"
#include "TowerBuildMenuPrefab.h"

#include "../InfernoUI.h"
#include "BaseTower.h"
#include "ResourceCounter.h"
#include "TowerBuildMenu.h"
#include "TowerDescription.h"
#include "TowerInfo.h"

// === Constructor / Destructor === //
TowerBuildMenuPrefab::TowerBuildMenuPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pBuildMenuImage = new Texture();
	m_pBuildMenuImage->SetImage(Inferno::GetImage(L"UI_BuildMenu_New.dds"));
	m_pBMLeftBtnImage = new Texture();
	m_pBMLeftBtnImage->SetImage(Inferno::GetImage(L"UI_BuildMenu_BtnL.dds"));
	m_pBMRightBtnImage = new Texture();
	m_pBMRightBtnImage->SetImage(Inferno::GetImage(L"UI_BuildMenu_BtnR.dds"));
	m_pAT_Icon = new Texture();
	m_pAT_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Auto.dds"));
	m_pLT_Icon = new Texture();
	m_pLT_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Lazer.dds"));
	m_pFT_Icon = new Texture();
	m_pFT_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Freeze.dds"));
	m_pM_Icon = new Texture();
	m_pM_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Mortar.dds"));
	m_pPT_Icon = new Texture();
	m_pPT_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Poison.dds"));
	m_pAT_W_Icon = new Texture();
	m_pAT_W_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Auto_WO.dds"));
	m_pLT_W_Icon = new Texture();
	m_pLT_W_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Lazer_WO.dds"));
	m_pFT_W_Icon = new Texture();
	m_pFT_W_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Freeze_WO.dds"));
	m_pM_W_Icon = new Texture();
	m_pM_W_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Mortar_WO.dds"));
	m_pPT_W_Icon = new Texture();
	m_pPT_W_Icon->SetImage(Inferno::GetImage(L"UI_Tow_Poison_WO.dds"));
	m_pResourceIcon = new Texture();
	m_pResourceIcon->SetImage(Inferno::GetImage(L"UI_ResourcesIcon.dds"));
	m_pControlsInfoImage = new Texture();
	m_pControlsInfoImage->SetImage(Inferno::GetImage(L"UI_ControlsInfo.dds"));
	m_pTowerLabelImage = new Texture();
	m_pTowerLabelImage->SetImage(Inferno::GetImage(L"UI_BuildMenu_Title.dds"));
}

TowerBuildMenuPrefab::~TowerBuildMenuPrefab()
{
	// delete Textures
	delete m_pBuildMenuImage;
	delete m_pBMLeftBtnImage;
	delete m_pBMRightBtnImage;
	delete m_pAT_Icon;
	delete m_pLT_Icon;
	delete m_pFT_Icon;
	delete m_pM_Icon;
	delete m_pPT_Icon;
	delete m_pAT_W_Icon;
	delete m_pLT_W_Icon;
	delete m_pFT_W_Icon;
	delete m_pM_W_Icon;
	delete m_pPT_W_Icon; 
	delete m_pResourceIcon;
	delete m_pControlsInfoImage;
	delete m_pTowerLabelImage;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("ExtendedHUD");
}
// ================================ //

// === Interface === //
void TowerBuildMenuPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void TowerBuildMenuPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform;
	UIRenderer* image;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

#if 0

#pragma region OLD_BUILD_MENU
	// === Tower Build Menu
	UIElement* towerBuildMenu = new UIElement();
	towerBuildMenu->SetName("Tower Build Menu");
	transform = towerBuildMenu->GetTransform();
	transform->SetPosition(0.0f, 53.0f);
	transform->SetBounds(Vector2(448.0f, 576.0f));
	//// Components
		// == UIRenderer
		image = towerBuildMenu->AddComponent<UIRenderer>();
		image->SetTexture(m_pBuildMenuImage);
		image->SetLayer(0);
		image->SetColor(Color(1.0f, 1.0f, 1.0f, 0.7f));
		// == TowerBuildMenu
		TowerBuildMenu* buildMenuScript = towerBuildMenu->AddComponent<TowerBuildMenu>();

	//// Children
	// === Left Tab
	UIElement* leftTab = new UIElement();
	leftTab->SetName("TBM Left Tab");
	transform = leftTab->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-74.0f, -217.5f);
	transform->SetBounds(Vector2(148.0f, 71.0f));
	//// Components
		// == UIButton
		UIButton* ltBtn = leftTab->AddComponent<UIButton>();
		ltBtn->SetNormalImage(m_pBMLeftBtnImage, Color(1.0f, 1.0f, 1.0f, 0.5f));
		ltBtn->SetHoverImage(m_pBMLeftBtnImage, Color(0.5f, 0.5f, 0.5f, 0.5f));
		ltBtn->SetLayer(2);
		leftTab->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnTurretTabClick, buildMenuScript));

	// === Right Tab
	UIElement* rightTab = new UIElement();
	rightTab->SetName("TBM Right Tab");
	transform = rightTab->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(74.0f, -217.5f);
	transform->SetBounds(Vector2(148.0f, 71.0f));
	//// Components
		// == UIButton
		UIButton* rtBtn = rightTab->AddComponent<UIButton>();
		rtBtn->SetNormalImage(m_pBMRightBtnImage, Color(1.0f, 1.0f, 1.0f, 0.5f));
		rtBtn->SetHoverImage(m_pBMRightBtnImage, Color(0.5f, 0.5f, 0.5f, 0.5f));
		rtBtn->SetLayer(2);
		rightTab->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnBuildingsTabClick, buildMenuScript));

	// === AutoTurret Btn
	UIElement* autoTurretBtn = new UIElement();
	autoTurretBtn->SetName("AutoTurret Btn");
	transform = autoTurretBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(0.0f, -136.8f);
	transform->SetBounds(Vector2(100.0f, 100.0f));
	//// Components
		// == UIButton
		UIButton* atBtn = autoTurretBtn->AddComponent<UIButton>();
		atBtn->SetNormalImage(m_pAT_W_Icon);
		atBtn->SetHoverImage(m_pAT_Icon);
		atBtn->SetLayer(4);
		autoTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnAutoTurretBtnClick, buildMenuScript));
		// == TowerInfo (AutoTurret)
		TowerInfo* tInfo = autoTurretBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_AUTOTURRET);
		tInfo->SetNameOffset(0.0f, -35.0f);
		tInfo->SetUnavailableOffset(-5.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 70.0f);

	// === LazerTurret Btn
	UIElement* lazerTurretBtn = new UIElement();
	lazerTurretBtn->SetName("LazerTurret Btn");
	transform = lazerTurretBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(123.6f, -40.1f);
	transform->SetBounds(Vector2(100.0f, 100.0f));
	//// Components
		// == UIButton
		UIButton* lztBtn = lazerTurretBtn->AddComponent<UIButton>();
		lztBtn->SetNormalImage(m_pLT_W_Icon);
		lztBtn->SetHoverImage(m_pLT_Icon);
		lztBtn->SetLayer(4);
		lazerTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnLazerTurretBtnClick, buildMenuScript));
		// == TowerInfo (LazerTurret)
		tInfo = lazerTurretBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_LAZERTURRET);
		tInfo->SetNameOffset(0.0f, -35.0f);
		tInfo->SetUnavailableOffset(-15.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 70.0f);
		tInfo->SetAsUnavailable(true);

	// === FreezeTower Btn
	UIElement* freezeTowerBtn = new UIElement();
	freezeTowerBtn->SetName("FreezeTower Btn");
	transform = freezeTowerBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(76.4f, 105.1f);
	transform->SetBounds(Vector2(100.0f, 100.0f));
	//// Components
		// == UIButton
		UIButton* ftBtn = freezeTowerBtn->AddComponent<UIButton>();
		ftBtn->SetNormalImage(m_pFT_W_Icon);
		ftBtn->SetHoverImage(m_pFT_Icon);
		ftBtn->SetLayer(4);
		freezeTowerBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnFreezeTowerBtnClick, buildMenuScript));
		// == TowerInfo (FreezeTower)
		tInfo = freezeTowerBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_FREEZETOWER);
		tInfo->SetNameOffset(0.0f, -35.0f);
		tInfo->SetUnavailableOffset(0.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 65.0f);

	// === Mortar Btn
	UIElement* mortarBtn = new UIElement();
	mortarBtn->SetName("Mortar Btn");
	transform = mortarBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-76.4f, 105.1f);
	transform->SetBounds(Vector2(100.0f, 100.0f));
	//// Components
		// == UIButton
		UIButton* mtBtn = mortarBtn->AddComponent<UIButton>();
		mtBtn->SetNormalImage(m_pM_W_Icon);
		mtBtn->SetHoverImage(m_pM_Icon);
		mtBtn->SetLayer(4);
		mortarBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnMortarBtnClick, buildMenuScript));
		// == TowerInfo (Mortar)
		tInfo = mortarBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_MORTAR);
		tInfo->SetNameOffset(0.0f, -35.0f);
		tInfo->SetUnavailableOffset(0.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 65.0f);

	// === PoisonTurret Btn
	UIElement* poisonTurretBtn = new UIElement();
	poisonTurretBtn->SetName("PoisonTurret Btn");
	transform = poisonTurretBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-123.6f, -40.1f);
	transform->SetBounds(Vector2(100.0f, 100.0f));
	//// Components
		// == UIButton
		UIButton* ptBtn = poisonTurretBtn->AddComponent<UIButton>();
		ptBtn->SetNormalImage(m_pPT_W_Icon);
		ptBtn->SetHoverImage(m_pPT_Icon);
		ptBtn->SetLayer(4);
		poisonTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnPoisonTurretClick, buildMenuScript));
		// == TowerInfo (PosionTurret)
		tInfo = poisonTurretBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_POISONTOWER);
		tInfo->SetNameOffset(0.0f, -35.0f);
		tInfo->SetUnavailableOffset(-15.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 70.0f);
//		tInfo->SetAsUnavailable(true);

	// === Resources Text
	UIElement* resourcesText = new UIElement();
	resourcesText->SetName("Resources Text");
	transform = resourcesText->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(22.0f, 230.0f);
	transform->SetBounds(Vector2(85.0f, 30.0f));
	//// Components
		// == UIText
		UIText* rTxt = resourcesText->AddComponent<UIText>();
		rTxt->SetText("0000");
		rTxt->SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
		rTxt->SetFontSize(24.0f);
		rTxt->SetLayer(4);
		// == Resource Counter
		resourcesText->AddComponent<ResourceCounter>();

	// === Resources Icon
	UIElement* resourcesIcon = new UIElement();
	resourcesIcon->SetName("Resources Icon");
	transform = resourcesIcon->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-50.0f, 230.0f);
	transform->SetBounds(Vector2(30.0f, 30.0f));
	//// Components
		// == UIImage
		image = resourcesIcon->AddComponent<UIRenderer>();
		image->SetTexture(m_pResourceIcon);
		image->SetColor(Color(0.2f, 1.0f, 0.2f, 1.0f));
		image->SetLayer(4);

	// == Controls Info
	UIElement* controlsImage = new UIElement();
	controlsImage->SetName("BM Controls Info");
	transform = controlsImage->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(0.0f, 285.0f);
	transform->SetBounds(Vector2(288.0f, 36.0f));
	//// Components
		// == UIRenderer (Controls Image)
		image = controlsImage->AddComponent<UIRenderer>();
		image->SetTexture(m_pControlsInfoImage);

	//// Post Setup
	atBtn->SetDownSelectable(ptBtn); atBtn->SetRightSelectable(lztBtn); atBtn->SetLeftSelectable(ptBtn);
	lztBtn->SetUpSelectable(atBtn); lztBtn->SetDownSelectable(ftBtn);
	ftBtn->SetUpSelectable(lztBtn); ftBtn->SetRightSelectable(lztBtn); ftBtn->SetLeftSelectable(mtBtn);
	mtBtn->SetUpSelectable(ptBtn); mtBtn->SetRightSelectable(ftBtn); mtBtn->SetLeftSelectable(ptBtn);
	ptBtn->SetUpSelectable(atBtn); ptBtn->SetDownSelectable(mtBtn); ptBtn->SetRightSelectable(mtBtn);
	buildMenuScript->SetFirstSelectable(atBtn);
#pragma endregion
#else
#pragma region NEW_MENU
	// === Tower Build Menu
	UIElement* towerBuildMenu = new UIElement();
	towerBuildMenu->SetName("Tower Build Menu");
	transform = towerBuildMenu->GetTransform();
	transform->SetPosition(0.0f, 53.0f);
	transform->SetBounds(Vector2(512.0f, 512.0f));
	//// Components
		// == UIRenderer
		image = towerBuildMenu->AddComponent<UIRenderer>();
		image->SetTexture(m_pBuildMenuImage);
		image->SetLayer(0);
		image->SetColor(Color(1.0f, 1.0f, 1.0f, 0.6f));
		// == TowerBuildMenu
		TowerBuildMenu* buildMenuScript = towerBuildMenu->AddComponent<TowerBuildMenu>();

	//// Children
	// === Tower's Label
	UIElement* towerLabel = new UIElement();
	towerLabel->SetName("Tower Label");
	transform = towerLabel->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-100.0f, -186.0f);
	transform->SetBounds(Vector2(160.0f * 1.1f, 32.0f * 1.1f));
	//// Components
		// == UIRenderer
		image = towerLabel->AddComponent<UIRenderer>();
		image->SetTexture(m_pTowerLabelImage);
		image->SetLayer(4);
		image->SetColor(Color(0.0f, 157.0f / 255.0f, 208.0f / 255.0f, 1.0f));

	// === AutoTurret Btn
	UIElement* autoTurretBtn = new UIElement();
	autoTurretBtn->SetName("AutoTurret Btn");
	transform = autoTurretBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-100.0f, -90.0f);
	transform->SetBounds(Vector2(125.0f, 125.0f));
	//// Components
		// == UIButton
		UIButton* atBtn = autoTurretBtn->AddComponent<UIButton>();
		atBtn->SetNormalImage(m_pAT_W_Icon);
		atBtn->SetHoverImage(m_pAT_Icon);
		atBtn->SetLayer(4);
		autoTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnAutoTurretBtnClick, buildMenuScript));
		autoTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, std::bind(&TowerBuildMenu::OnAutoTurretHover, buildMenuScript));
		autoTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		autoTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONSELECT, std::bind(&TowerBuildMenu::OnAutoTurretHover, buildMenuScript));
		autoTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONDESELECT, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		// == TowerInfo (AutoTurret)
		TowerInfo* tInfo = autoTurretBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_AUTOTURRET);
		tInfo->SetNameOffset(0.0f, -45.0f);
		tInfo->SetUnavailableOffset(-5.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 65.0f);

	// === FreezeTower Btn
	UIElement* freezeTowerBtn = new UIElement();
	freezeTowerBtn->SetName("FreezeTower Btn");
	transform = freezeTowerBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(100.0f, -90.0f);
	transform->SetBounds(Vector2(100.0f, 100.0f));
	//// Components
		// == UIButton
		UIButton* ftBtn = freezeTowerBtn->AddComponent<UIButton>();
		ftBtn->SetNormalImage(m_pFT_W_Icon);
		ftBtn->SetHoverImage(m_pFT_Icon);
		ftBtn->SetLayer(4);
		freezeTowerBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnFreezeTowerBtnClick, buildMenuScript));
		freezeTowerBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, std::bind(&TowerBuildMenu::OnFreezeTurretHover, buildMenuScript));
		freezeTowerBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		freezeTowerBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONSELECT, std::bind(&TowerBuildMenu::OnFreezeTurretHover, buildMenuScript));
		freezeTowerBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONDESELECT, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		// == TowerInfo (FreezeTower)
		tInfo = freezeTowerBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_FREEZETOWER);
		tInfo->SetNameOffset(0.0f, -45.0f);
		tInfo->SetUnavailableOffset(0.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 65.0f);

	// === Mortar Btn
	UIElement* mortarBtn = new UIElement();
	mortarBtn->SetName("Mortar Btn");
	transform = mortarBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-100.0f, 60.0f);
	transform->SetBounds(Vector2(125.0f, 125.0f));
	//// Components
		// == UIButton
		UIButton* mtBtn = mortarBtn->AddComponent<UIButton>();
		mtBtn->SetNormalImage(m_pM_W_Icon);
		mtBtn->SetHoverImage(m_pM_Icon);
		mtBtn->SetLayer(4);
		mortarBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnMortarBtnClick, buildMenuScript));
		mortarBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, std::bind(&TowerBuildMenu::OnMortarTurretHover, buildMenuScript));
		mortarBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		mortarBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONSELECT, std::bind(&TowerBuildMenu::OnMortarTurretHover, buildMenuScript));
		mortarBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONDESELECT, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		// == TowerInfo (Mortar)
		tInfo = mortarBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_MORTAR);
		tInfo->SetNameOffset(0.0f, -45.0f);
		tInfo->SetUnavailableOffset(0.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 65.0f);

	// === PoisonTurret Btn
	UIElement* poisonTurretBtn = new UIElement();
	poisonTurretBtn->SetName("PoisonTurret Btn");
	transform = poisonTurretBtn->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(100.0f, 60.0f);
	transform->SetBounds(Vector2(125.0f, 125.0f));
	//// Components
		// == UIButton
		UIButton* ptBtn = poisonTurretBtn->AddComponent<UIButton>();
		ptBtn->SetNormalImage(m_pPT_W_Icon);
		ptBtn->SetHoverImage(m_pPT_Icon);
		ptBtn->SetLayer(4);
		poisonTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&TowerBuildMenu::OnPoisonTurretClick, buildMenuScript));
		poisonTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVERBEGIN, std::bind(&TowerBuildMenu::OnPoisonTurretHover, buildMenuScript));
		poisonTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONHOVEREND, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		poisonTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONSELECT, std::bind(&TowerBuildMenu::OnPoisonTurretHover, buildMenuScript));
		poisonTurretBtn->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONDESELECT, std::bind(&TowerBuildMenu::OnTowerBtnHoverEnd, buildMenuScript));
		// == TowerInfo (PosionTurret)
		tInfo = poisonTurretBtn->AddComponent<TowerInfo>();
		tInfo->SetType(TowerTypes::TT_POISONTOWER);
		tInfo->SetNameOffset(0.0f, -45.0f);
		tInfo->SetUnavailableOffset(-15.0f, 10.0f);
		tInfo->SetPriceOffset(0.0f, 65.0f);

	// === Resources Text
	UIElement* resourcesText = new UIElement();
	resourcesText->SetName("Resources Text");
	transform = resourcesText->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(70.0f, 185.0f);
	transform->SetBounds(Vector2(150.0f, 30.0f));
	//// Components
		// == UIText
		UIText* rTxt = resourcesText->AddComponent<UIText>();
		rTxt->SetText("0000");
		rTxt->SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
		rTxt->SetFontSize(28.0f);
		rTxt->SetLayer(4);
		// == Resource Counter
		resourcesText->AddComponent<ResourceCounter>();

	// === Resources Icon
	UIElement* resourcesIcon = new UIElement();
	resourcesIcon->SetName("Resources Icon");
	transform = resourcesIcon->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(-35.0f, 185.0f);
	transform->SetBounds(Vector2(56.0f, 56.0f));
	//// Components
		// == UIImage
		image = resourcesIcon->AddComponent<UIRenderer>();
		image->SetTexture(m_pResourceIcon);
		image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		image->SetLayer(4);

	// == Tower Description
	UIElement* descriptionElement = new UIElement();
	descriptionElement->SetName("Tower Description Element");
	transform = descriptionElement->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetBounds(Vector2(320.0f, 128.0f));
	//// Components
		// == Tower Description 
		descriptionElement->AddComponent<TowerDescription>();

	// == Controls Info
	UIElement* controlsImage = new UIElement();
	controlsImage->SetName("BM Controls Info");
	transform = controlsImage->GetTransform();
	transform->SetParent(towerBuildMenu->GetTransform());
	transform->SetPosition(0.0f, 285.0f);
	transform->SetBounds(Vector2(288.0f, 36.0f));
	//// Components
		// == UIRenderer (Controls Image)
		image = controlsImage->AddComponent<UIRenderer>();
		image->SetTexture(m_pControlsInfoImage);

	//// Post Setup
	atBtn->SetDownSelectable(mtBtn); atBtn->SetRightSelectable(ftBtn); atBtn->SetLeftSelectable(ftBtn); atBtn->SetUpSelectable(mtBtn);
	ftBtn->SetUpSelectable(ptBtn); ftBtn->SetRightSelectable(atBtn); ftBtn->SetLeftSelectable(atBtn); ftBtn->SetDownSelectable(ptBtn);
	mtBtn->SetUpSelectable(atBtn); mtBtn->SetRightSelectable(ptBtn); mtBtn->SetLeftSelectable(ptBtn); mtBtn->SetDownSelectable(atBtn);
	ptBtn->SetUpSelectable(ftBtn); ptBtn->SetDownSelectable(ftBtn); ptBtn->SetRightSelectable(mtBtn); ptBtn->SetLeftSelectable(mtBtn);
	buildMenuScript->SetFirstSelectable(atBtn);
#pragma endregion
#endif

	AddToView(towerBuildMenu, "ExtendedHUD");
	towerBuildMenu->SetActive(false);
}
// ================= //

// === Protected Interface === //
IComponent& TowerBuildMenuPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

TowerBuildMenuPrefab& TowerBuildMenuPrefab::operator=(const TowerBuildMenuPrefab& _rhs)
{
	return *this;
}

IComponent* TowerBuildMenuPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new TowerBuildMenuPrefab(gameObject, transform);
}
// =========================== //