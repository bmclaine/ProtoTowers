#include "stdafx.h"
#include "OptionsMenuPrefab.h"
#include "OptionsMenuScript.h"
#include "ModuleInterface.h"
#include "../InfernoUI.h"

#include <iomanip>
#include <string>
#include <sstream>

OptionsMenuPrefab::OptionsMenuPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	int OTB = Inferno::GetImage(L"OptionsTextBackground");
	int OMB = Inferno::GetImage(L"OptionsMenuBackground");
	int OAB = Inferno::GetImage(L"OptionsAcceptBackground");

	if (OTB == -1)
		OTB = InfernoEngine_Graphics::CreateSolidColorImage(L"OptionsTextBackground", 920, 100, 2, Color(0.0f, 0.0f, 0.0f, 0.4f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	if (OMB == -1)
		OMB = InfernoEngine_Graphics::CreateSolidColorImage(L"OptionsMenuBackground", 920, 576, 2, Color(0.0f, 0.0f, 0.0f, 0.4f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	if (OAB == -1)
		OAB = InfernoEngine_Graphics::CreateSolidColorImage(L"OptionsMenuBackground", 200, 150, 2, Color(0.0f, 0.0f, 0.0f, 0.4f), Color(0.0f, 0.0f, 0.0f, 1.0f));

	m_pBtnImageN = new Texture();
	m_pBtnImageN->SetImage(Inferno::GetImage(L"UI_MM_ButtonN.dds"));
	m_pBtnImageS = new Texture();
	m_pBtnImageS->SetImage(Inferno::GetImage(L"UI_MM_ButtonS.dds"));
	m_pDpDwnImageN = new Texture();
	m_pDpDwnImageN->SetImage(Inferno::GetImage(L"UI_DropdownArrow_Unselected_Image.dds"));
	m_pDpDwnImageH = new Texture();
	m_pDpDwnImageH->SetImage(Inferno::GetImage(L"UI_DropdownArrow_Hovered_Image.dds"));
	m_pDpDwnImageS = new Texture();
	m_pDpDwnImageS->SetImage(Inferno::GetImage(L"UI_DropdownArrow_Selected_Image.dds"));
	m_pResolutionText = new Texture();
	m_pResolutionText->SetImage(Inferno::GetImage(L"UI_Resolution_Text.dds"));
	m_pWindowText = new Texture();
	m_pWindowText->SetImage(Inferno::GetImage(L"UI_Window_Text.dds"));
	m_pApplyButtonText = new Texture();
	m_pApplyButtonText->SetImage(Inferno::GetImage(L"UI_Apply_Button.dds"));
	m_pBackButtonText = new Texture();
	m_pBackButtonText->SetImage(Inferno::GetImage(L"UI_Back_Button.dds"));
	m_pDefaultButtonText = new Texture();
	m_pDefaultButtonText->SetImage(Inferno::GetImage(L"UI_Default_Text.dds"));
	m_pOKButtonText = new Texture();
	m_pOKButtonText->SetImage(Inferno::GetImage(L"UI_OK_Text.dds"));;
	m_pCancelButtonText = new Texture();
	m_pCancelButtonText->SetImage(Inferno::GetImage(L"UI_Cancel_Text.dds"));;
	m_pOptionsText = new Texture();
	m_pOptionsText->SetImage(Inferno::GetImage(L"UI_Options_Text.dds"));
	m_pGraphicsText = new Texture();
	m_pGraphicsText->SetImage(Inferno::GetImage(L"UI_Graphics_Text.dds"));
	m_pAudioText = new Texture();
	m_pAudioText->SetImage(Inferno::GetImage(L"UI_Audio_Text.dds"));
	m_pOptionsTextBackground = new Texture();
	m_pOptionsTextBackground->SetImage(OTB);
	m_pOptionsMenuBackground = new Texture();
	m_pOptionsMenuBackground->SetImage(OMB);
	m_pOptionsAcceptBackground = new Texture();
	m_pOptionsAcceptBackground->SetImage(OAB);

	m_sPrevCanvas = "";
}

OptionsMenuPrefab::~OptionsMenuPrefab()
{
	delete m_pBtnImageN;
	delete m_pBtnImageS;
	delete m_pDpDwnImageN;
	delete m_pDpDwnImageH;
	delete m_pDpDwnImageS;
	delete m_pResolutionText;
	delete m_pWindowText;
	delete m_pApplyButtonText;
	delete m_pBackButtonText;
	delete m_pDefaultButtonText;
	delete m_pOKButtonText;
	delete m_pCancelButtonText;
	delete m_pOptionsText;
	delete m_pGraphicsText;
	delete m_pAudioText;
	delete m_pOptionsTextBackground;
	delete m_pOptionsMenuBackground;
	delete m_pOptionsAcceptBackground;
}

OptionsMenuPrefab& OptionsMenuPrefab::operator=(const OptionsMenuPrefab& _rhs)
{
	return *this;
}
IComponent* OptionsMenuPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	OptionsMenuPrefab* OMP = new OptionsMenuPrefab(gameObject, transform);

	return OMP;
}

void OptionsMenuPrefab::Init()
{
	
}
void OptionsMenuPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	if (data)
	{
		std::string propName = "PreviousCanvas";
		Property* prop = data->GetProperty(propName);

		if (prop)
			m_sPrevCanvas = prop->value;
	}
}
void OptionsMenuPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* optionsCanvas = nullptr;
	unsigned int optionsCanvasSlot;

	optionsCanvas = GetCanvasByName("Options");
	if (optionsCanvas == nullptr) {
		CreateNewCanvas("Options", &optionsCanvas, &optionsCanvasSlot);
		optionsCanvas = GetCanvasByName("Options");
	}

	UIElement* ButtonsElement = new UIElement();
	OptionsMenuScript* optionsScript = ButtonsElement->AddComponent<OptionsMenuScript>();
	optionsScript->SetPreviousCanvas(m_sPrevCanvas);

	AddToView(ButtonsElement, "Options");

	// Options Menu Title
	UIElement* Title = new UIElement();
	Title->SetName("Menu Title");
	RectTransform* transform = Title->GetTransform();
	transform->SetBounds(Vector2(920, 100));
	transform->SetPosition(Vector2(0, -296));

	UIRenderer* image = Title->AddComponent<UIRenderer>();
	image->SetTexture(m_pOptionsTextBackground);
	image->SetLayer(5);

	UIElement* OptionsText = new UIElement();
	OptionsText->SetName("Options Text");
	transform = OptionsText->GetTransform();
	transform->SetParent(Title->GetTransform());
	transform->SetBounds(Vector2(192, 64));

	image = OptionsText->AddComponent<UIRenderer>();
	image->SetTexture(m_pOptionsText);
	image->SetLayer(6);

	AddToView(Title, "Options");

	// Background Image
	UIElement* Background = new UIElement();
	Background->SetName("Background");
	transform = Background->GetTransform();
	transform->SetBounds(Vector2(920, 576));
	transform->SetPosition(Vector2(0, 57));

	image = Background->AddComponent<UIRenderer>();
	image->SetTexture(m_pOptionsMenuBackground);
	image->SetLayer(5);

	// Graphics Text
	UIElement* GraphicsText = new UIElement();
	GraphicsText->SetName("Graphics Text");
	transform = GraphicsText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(0, -236));
	transform->SetBounds(Vector2(192, 64));

	image = GraphicsText->AddComponent<UIRenderer>();
	image->SetTexture(m_pGraphicsText);
	image->SetLayer(6);

	// Resolution Text
	UIElement* ResolutionText = new UIElement();
	ResolutionText->SetName("Resolution Text");
	transform = ResolutionText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, -200));
	transform->SetBounds(Vector2(192, 64));

	UIText* text = ResolutionText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"Resolution");

	//image = ResolutionText->AddComponent<UIRenderer>();
	//image->SetTexture(m_pResolutionText);
	//image->SetLayer(6);

	// Resolution Dropdown
	UIElement* ResolutionDropdown = new UIElement();
	ResolutionDropdown->SetName("Resolution Dropdown");
	transform = ResolutionDropdown->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(289, -200));
	transform->SetBounds(Vector2(192, 32));

	Vector2 WindowDim = InfernoEngine_Graphics::GetWindowDimensions();
	UIDropdown* ResDropdown = ResolutionDropdown->AddComponent<UIDropdown>();
	ResDropdown->SetLayer(6);
	// Set the starting text to the current resolution of the game
	ResDropdown->SetStartingValue(std::to_wstring((UINT)WindowDim.x) + L"x" + std::to_wstring((UINT)WindowDim.y));
	// Set the supported resolutions into the dropdown
	{
		std::vector<std::wstring> resolutions;
		InfernoEngine_Graphics::GetSupportedResolutions(resolutions);
		ResDropdown->SetSelectableValues(resolutions);
		ResDropdown->SetFontAlignment(CENTER);
	}

	optionsScript->SetResolutionDropdown(ResDropdown);

	// Window Text
	UIElement* WindowText = new UIElement();
	WindowText->SetName("Window Text");
	transform = WindowText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, -157));
	transform->SetBounds(Vector2(192, 64));

	text = WindowText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"Window Style");

	//image = WindowText->AddComponent<UIRenderer>();
	//image->SetTexture(m_pWindowText);
	//image->SetLayer(6);

	// Window Dropdown
	UIElement* WindowDropdown = new UIElement();
	WindowDropdown->SetName("WindowDropdown");
	transform = WindowDropdown->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(289, -157));
	transform->SetBounds(Vector2(192, 32));

	UIDropdown* WinDropdown = WindowDropdown->AddComponent<UIDropdown>();
	{
		std::vector<std::wstring> styles;
		styles.push_back(L"Windowed");
		styles.push_back(L"Borderless");
		styles.push_back(L"Fullscreen");
		WinDropdown->SetSelectableValues(styles);
	}
	WinDropdown->SetFontAlignment(CENTER);
	WinDropdown->SetStartingValue(InfernoEngine_Graphics::GetWindowStyle());
	WinDropdown->SetLayer(6);
	WinDropdown->SetFunctionToCallOnSelectionChange(std::bind(&OptionsMenuScript::OnWindowStyleChange, optionsScript));

	optionsScript->SetWindowDropdown(WinDropdown);

	// VSync Text
	UIElement* VSyncText = new UIElement();
	VSyncText->SetName("VSync Text");
	transform = VSyncText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, -114));
	transform->SetBounds(Vector2(192, 64));

	text = VSyncText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"VSync");

	// VSync Checkbox
	UIElement* VSyncCheckbox = new UIElement();
	VSyncCheckbox->SetName("VSync Checkbox");
	transform = VSyncCheckbox->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(369, -114));
	transform->SetBounds(Vector2(32, 32));

	UICheckbox* VCheckbox = VSyncCheckbox->AddComponent<UICheckbox>();
	VCheckbox->SetChecked(InfernoEngine_Graphics::VSyncEnabled());
	VCheckbox->SetLayer(6);

	optionsScript->SetVSyncCheckbox(VCheckbox);

	// Anisotropic Text
	UIElement* AnisotropicText = new UIElement();
	AnisotropicText->SetName("Anisotropic Text");
	transform = AnisotropicText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, -71));
	transform->SetBounds(Vector2(250, 64));

	text = AnisotropicText->AddComponent<UIText>();
	text->SetFontSize(24.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"Anisotropic Filtering");

	// Anisotropic Dropdown
	UIElement* AnisotropicDropdown = new UIElement();
	AnisotropicDropdown->SetName("Anisotropic Dropdown");
	transform = AnisotropicDropdown->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(353, -71));
	transform->SetBounds(Vector2(64, 32));

	UIDropdown* AniDropdown = AnisotropicDropdown->AddComponent<UIDropdown>();
	{
		std::vector<std::wstring> styles;
		styles.push_back(L"1");
		styles.push_back(L"2");
		styles.push_back(L"4");
		styles.push_back(L"8");
		styles.push_back(L"16");
		AniDropdown->SetSelectableValues(styles);
	}
	AniDropdown->SetFontAlignment(CENTER);
	AniDropdown->SetStartingValue(std::to_wstring(InfernoEngine_Graphics::GetAnisotropicFiltering()));
	AniDropdown->SetLayer(6);

	optionsScript->SetAnisotropicDropdown(AniDropdown);

	// Gamma Text
	UIElement* GammaText = new UIElement();
	GammaText->SetName("Gamma Text");
	transform = GammaText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, -27));
	transform->SetBounds(Vector2(250, 64));

	text = GammaText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"Gamma");

	// Gamma Slider
	UIElement* gammaSlider = new UIElement();
	gammaSlider->SetName("Gamma Slider");
	transform = gammaSlider->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(289, -27));
	transform->SetBounds(Vector2(192, 40));

	float gamma = InfernoEngine_Graphics::GetGamma();
	UISlider* GammaSlider = gammaSlider->AddComponent<UISlider>();
	GammaSlider->SetBackImageColor(Color(1, 1, 1, 1));
	GammaSlider->SetFillImageColor(Color(1, 0, 0, 1));
	GammaSlider->SetSelectedColor(Color(1, 1, 1, 1));
	GammaSlider->SetHoveredColor(Color(1, 1, 1, 0.5f));
	GammaSlider->SetLayer(6);
	GammaSlider->SetFill((gamma - 0.2f) * 0.4f);
	GammaSlider->SetOnFillChangeFunction(std::bind(&OptionsMenuScript::OnGammaSliderChange, optionsScript));

	optionsScript->SetGammaSlider(GammaSlider);

	// Gamma Level Text
	UIElement* GammaLevelText = new UIElement();
	GammaLevelText->SetName("Gamma Level Text");
	transform = GammaLevelText->GetTransform();
	transform->SetParent(gammaSlider->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_CENTERRIGHT);
	transform->SetPosition(Vector2(32, 0));
	transform->SetBounds(Vector2(64, 64));

	std::wstringstream stream;
	stream << std::fixed << std::setprecision(2) << gamma;

	text = GammaLevelText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(stream.str());

	optionsScript->SetGammaText(text);

	// Audio Text
	UIElement* AudioText = new UIElement();
	AudioText->SetName("Audio Text");
	transform = AudioText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(0, 25));
	transform->SetBounds(Vector2(192, 64));

	image = AudioText->AddComponent<UIRenderer>();
	image->SetTexture(m_pAudioText);
	image->SetLayer(6);

	// Master Volume Text
	UIElement* MasterVolumeText = new UIElement();
	MasterVolumeText->SetName("Master Volume Text");
	transform = MasterVolumeText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, 61));
	transform->SetBounds(Vector2(220, 32));

	text = MasterVolumeText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"Master Volume");

	// Master Volume Slider
	UIElement* MasterSlider = new UIElement();
	MasterSlider->SetName("Master Volume Slider");
	transform = MasterSlider->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(289, 61));
	transform->SetBounds(Vector2(192, 40));

	float mVolume = InfernoEngine_Audio::GetMasterVolume() * 100.0f;
	UISlider* MasterVolumeSlider = MasterSlider->AddComponent<UISlider>();
	MasterVolumeSlider->SetBackImageColor(Color(1, 1, 1, 1));
	MasterVolumeSlider->SetFillImageColor(Color(1, 0, 0, 1));
	MasterVolumeSlider->SetSelectedColor(Color(1, 1, 1, 1));
	MasterVolumeSlider->SetHoveredColor(Color(1, 1, 1, 0.5f));
	MasterVolumeSlider->SetLayer(6);
	MasterVolumeSlider->SetFill(mVolume * 0.01f);
	MasterVolumeSlider->SetOnFillChangeFunction(std::bind(&OptionsMenuScript::OnMasterVolumeSliderChange, optionsScript));

	optionsScript->SetMasterVolumeSlider(MasterVolumeSlider);

	// Master Volume Level Text
	UIElement* MasterLevelText = new UIElement();
	MasterLevelText->SetName("Master Volume Level Text");
	transform = MasterLevelText->GetTransform();
	transform->SetParent(MasterSlider->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_CENTERRIGHT);
	transform->SetPosition(Vector2(32, 0));
	transform->SetBounds(Vector2(64, 64));

	text = MasterLevelText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(std::to_wstring((UINT)mVolume));

	optionsScript->SetMasterVolumeText(text);

	// Music Volume Text
	UIElement* MusicVolumeText = new UIElement();
	MusicVolumeText->SetName("Music Volume Text");
	transform = MusicVolumeText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, 104));
	transform->SetBounds(Vector2(192, 32));

	text = MusicVolumeText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"Music Volume");

	// Music Volume Slider
	UIElement* MusicSlider = new UIElement();
	MusicSlider->SetName("Music Volue Slider");
	transform = MusicSlider->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(289, 104));
	transform->SetBounds(Vector2(192, 40));

	float musVolume = InfernoEngine_Audio::GetBackgroundMusicVolume() * 100.0f;
	UISlider* MusicVolumeSlider = MusicSlider->AddComponent<UISlider>();
	MusicVolumeSlider->SetBackImageColor(Color(1, 1, 1, 1));
	MusicVolumeSlider->SetFillImageColor(Color(1, 0, 0, 1));
	MusicVolumeSlider->SetSelectedColor(Color(1, 1, 1, 1));
	MusicVolumeSlider->SetHoveredColor(Color(1, 1, 1, 0.5f));
	MusicVolumeSlider->SetLayer(6);
	MusicVolumeSlider->SetFill(musVolume * 0.01f);
	MusicVolumeSlider->SetOnFillChangeFunction(std::bind(&OptionsMenuScript::OnMusicVolumeSliderChange, optionsScript));

	optionsScript->SetMusicVolumeSlider(MusicVolumeSlider);

	// Music Volume Level Text
	UIElement* MusicLevelText = new UIElement();
	MusicLevelText->SetName("Music Volume Level Text");
	transform = MusicLevelText->GetTransform();
	transform->SetParent(MusicSlider->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_CENTERRIGHT);
	transform->SetPosition(Vector2(32, 0));
	transform->SetBounds(Vector2(64, 64));

	text = MusicLevelText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(std::to_wstring((UINT)musVolume));

	optionsScript->SetMusicVolumeText(text);

	// SFX Volume Text
	UIElement* SFXVolumeText = new UIElement();
	SFXVolumeText->SetName("SFX Volume Text");
	transform = SFXVolumeText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, 147));
	transform->SetBounds(Vector2(192, 32));

	text = SFXVolumeText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(L"SFX Volume");

	// SFX Volume Slider
	UIElement* SFXSlider = new UIElement();
	SFXSlider->SetName("SFX Volume Slider");
	transform = SFXSlider->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(289, 147));
	transform->SetBounds(Vector2(192, 40));

	float sfxVolume = InfernoEngine_Audio::GetSFXVolume() * 100.0f;
	UISlider* SFXVolumeSlider = SFXSlider->AddComponent<UISlider>();
	SFXVolumeSlider->SetBackImageColor(Color(1, 1, 1, 1));
	SFXVolumeSlider->SetFillImageColor(Color(1, 0, 0, 1));
	SFXVolumeSlider->SetSelectedColor(Color(1, 1, 1, 1));
	SFXVolumeSlider->SetHoveredColor(Color(1, 1, 1, 0.5f));
	SFXVolumeSlider->SetLayer(6);
	SFXVolumeSlider->SetFill(sfxVolume * 0.01f);
	SFXVolumeSlider->SetOnFillChangeFunction(std::bind(&OptionsMenuScript::OnSFXVolumeSliderChange, optionsScript));

	optionsScript->SetSFXVolumeSlider(SFXVolumeSlider);

	// SFX Volume Level Text
	UIElement* SFXLevelText = new UIElement();
	SFXLevelText->SetName("SFX Volume Level Text");
	transform = SFXLevelText->GetTransform();
	transform->SetParent(SFXSlider->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_CENTERRIGHT);
	transform->SetPosition(Vector2(32, 0));
	transform->SetBounds(Vector2(64, 64));

	text = SFXLevelText->AddComponent<UIText>();
	text->SetFontSize(30.0f);
	text->SetAlignment(CENTER);
	text->SetParagraphAlignment(PARAGRAPH_CENTER);
	text->SetLayer(6);
	text->SetText(std::to_wstring((UINT)sfxVolume));

	optionsScript->SetSFXVolumeText(text);

	// Back Button
	UIElement* BackButton = new UIElement();
	BackButton->SetName("Back Button");
	transform = BackButton->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(344, 236));
	transform->SetBounds(Vector2(150, 105));

	UIButton* BButton = BackButton->AddComponent<UIButton>();
	BButton->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	BButton->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	BButton->SetLayer(6);
	BackButton->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&OptionsMenuScript::BackBtn_OnClick, optionsScript));

	UIElement* backText = new UIElement();
	backText->SetName("Back Text");
	transform = backText->GetTransform();
	transform->SetParent(BackButton->GetTransform());
	transform->SetBounds(Vector2(70, 40));

	image = backText->AddComponent<UIRenderer>();
	image->SetTexture(m_pBackButtonText);
	image->SetLayer(7);

	// Apply Button
	UIElement* ApplyButton = new UIElement();
	ApplyButton->SetName("Apply Button");
	transform = ApplyButton->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(190, 236));
	transform->SetBounds(Vector2(150, 105));

	UIButton* AButton = ApplyButton->AddComponent<UIButton>();
	AButton->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	AButton->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	AButton->SetLayer(6);
	ApplyButton->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&OptionsMenuScript::ApplyBtn_OnClick, optionsScript));

	UIElement* applyText = new UIElement();
	applyText->SetName("Apply Text");
	transform = applyText->GetTransform();
	transform->SetParent(ApplyButton->GetTransform());
	transform->SetBounds(Vector2(70, 40));

	image = applyText->AddComponent<UIRenderer>();
	image->SetTexture(m_pApplyButtonText);
	image->SetLayer(6);

	// Default Button
	UIElement* DefaultButton = new UIElement();
	DefaultButton->SetName("Default Button");
	transform = DefaultButton->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetPosition(Vector2(-289, 236));
	transform->SetBounds(Vector2(150, 105));

	UIButton* DButton = DefaultButton->AddComponent<UIButton>();
	DButton->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	DButton->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	DButton->SetLayer(6);
	DefaultButton->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&OptionsMenuScript::DefaultBtn_OnClick, optionsScript));

	UIElement* defaultText = new UIElement();
	defaultText->SetName("Default Text");
	transform = defaultText->GetTransform();
	transform->SetParent(DefaultButton->GetTransform());
	transform->SetBounds(Vector2(100, 30));

	image = defaultText->AddComponent<UIRenderer>();
	image->SetTexture(m_pDefaultButtonText);
	image->SetLayer(6);

	AddToView(Background, "Options");

	// Accept Menu
	UIElement* AcceptMenu = new UIElement();
	transform = AcceptMenu->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_CENTER);
	transform->SetBounds(Vector2(300, 150));
	optionsScript->SetAcceptMenuElement(AcceptMenu);

	image = AcceptMenu->AddComponent<UIRenderer>();
	image->SetTexture(m_pOptionsAcceptBackground);
	image->SetLayer(15);

	// Accept Text
	UIElement* AcceptText = new UIElement();
	transform = AcceptText->GetTransform();
	transform->SetParent(AcceptMenu->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(200, 30));
	transform->SetPosition(Vector2(0, 25));

	text = AcceptText->AddComponent<UIText>();
	text->SetFontSize(18.0f);
	text->SetAlignment(TextAlignment::CENTER);
	text->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
	text->SetLayer(16);
	text->SetText(L"Accept the changes?");

	// Time Remaining Text
	UIElement* TimeRemainingText = new UIElement();
	transform = TimeRemainingText->GetTransform();
	transform->SetParent(AcceptMenu->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(200, 30));
	transform->SetPosition(Vector2(-15, 45));

	text = TimeRemainingText->AddComponent<UIText>();
	text->SetFontSize(18.0f);
	text->SetAlignment(TextAlignment::CENTER);
	text->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
	text->SetLayer(16);
	text->SetText(L"Time Remaining:");

	// Time Text
	UIElement* TimeText = new UIElement();
	transform = TimeText->GetTransform();
	transform->SetParent(AcceptMenu->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(50, 30));
	transform->SetPosition(Vector2(85, 45));

	text = TimeText->AddComponent<UIText>();
	text->SetFontSize(18.0f);
	text->SetAlignment(TextAlignment::LEADING);
	text->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
	text->SetLayer(16);
	text->SetText(L"15");
	optionsScript->SetAcceptTimeText(text);

	// OK Button
	UIElement* OKButton = new UIElement();
	transform = OKButton->GetTransform();
	transform->SetParent(AcceptMenu->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(120, 84));
	transform->SetPosition(Vector2(-65.0f, 108.0f));

	UIButton* OButton = OKButton->AddComponent<UIButton>();
	OButton->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	OButton->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	OButton->SetLayer(16);
	OKButton->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&OptionsMenuScript::OKBtn_OnClick, optionsScript));
	optionsScript->SetAcceptInitialSelectable(OButton);

	UIElement* okText = new UIElement();
	transform = okText->GetTransform();
	transform->SetParent(OKButton->GetTransform());
	transform->SetBounds(Vector2(60, 30));

	image = okText->AddComponent<UIRenderer>();
	image->SetTexture(m_pOKButtonText);
	image->SetLayer(17);

	// Cancel Button
	UIElement* CancelButton = new UIElement();
	transform = CancelButton->GetTransform();
	transform->SetParent(AcceptMenu->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(120, 84));
	transform->SetPosition(Vector2(65.0f, 108.0f));

	UIButton* CButton = CancelButton->AddComponent<UIButton>();
	CButton->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	CButton->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	CButton->SetLayer(16);
	CancelButton->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&OptionsMenuScript::CancelBtn_OnClick, optionsScript));

	UIElement* cancelText = new UIElement();
	transform = cancelText->GetTransform();
	transform->SetParent(CancelButton->GetTransform());
	transform->SetBounds(Vector2(60, 30));

	image = cancelText->AddComponent<UIRenderer>();
	image->SetTexture(m_pCancelButtonText);
	image->SetLayer(17);

	AddToView(AcceptMenu, "Options");

	optionsCanvas->GetUIElement()->SetActive(false);
	AcceptMenu->SetActive(false);

	optionsScript->SetInitialSelectable(ResDropdown);

	// Set Up the selectables ordering
	ResDropdown->SetDownSelectable(WinDropdown);
	WinDropdown->SetUpSelectable(ResDropdown);
	WinDropdown->SetDownSelectable(VCheckbox);
	VCheckbox->SetUpSelectable(WinDropdown);
	VCheckbox->SetDownSelectable(AniDropdown);
	AniDropdown->SetUpSelectable(VCheckbox);
	AniDropdown->SetDownSelectable(GammaSlider);
	GammaSlider->SetUpSelectable(AniDropdown);
	GammaSlider->SetDownSelectable(MasterVolumeSlider);
	MasterVolumeSlider->SetUpSelectable(GammaSlider);
	MasterVolumeSlider->SetDownSelectable(MusicVolumeSlider);
	MusicVolumeSlider->SetUpSelectable(MasterVolumeSlider);
	MusicVolumeSlider->SetDownSelectable(SFXVolumeSlider);
	SFXVolumeSlider->SetUpSelectable(MusicVolumeSlider);
	SFXVolumeSlider->SetDownSelectable(AButton);
	AButton->SetUpSelectable(SFXVolumeSlider);
	AButton->SetLeftSelectable(DButton);
	AButton->SetRightSelectable(BButton);
	BButton->SetUpSelectable(SFXVolumeSlider);
	BButton->SetLeftSelectable(AButton);
	DButton->SetRightSelectable(AButton);
	DButton->SetUpSelectable(SFXVolumeSlider);

	OButton->SetRightSelectable(CButton);
	OButton->SetLeftSelectable(CButton);
	CButton->SetRightSelectable(OButton);
	CButton->SetLeftSelectable(OButton);
}

void OptionsMenuPrefab::SetPreviousCanvas(std::string name)
{
	m_sPrevCanvas = name;
}