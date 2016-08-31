#include "InfernoEngine.h"

#include "GraphicsManager.h"
#include "ObjectManager.h"
#include "Physics\Physics.h"
#include "CameraManager.h"
#include "AssetManager.h"
#include "InputManager.h"
#include "Font\FontManager.h"
#include "UIManager.h"
#include "UIElement.h"
#include "UIComponents\UICanvas.h"
#include "UIComponents\UIImage.h"
#include "UIComponents\UIText.h"
#include "InfernoTime.h"
#include "EventSystem.h"
#include "AI\AIModule.h"
#include "SceneManager.h"
#include "ThreadPool\WorkerThreadPool.h"
#include "ThreadPool\WorkerTypes\WorkerThreadFuctionPointers.h"

#include "FBXLoader.h"
#include "GlobalIncludes.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "Components\Camera.h"
#include "Components\MeshRenderer.h"
#include "Assets\Material.h"
#include "WWiseResources\Wwise_IDs.h"
#include "FileSystem.h"
#include "Assets\Material.h"

#include "../../Interface/Public/Engine.h"
#include "../../Interface/Public/ModuleInterface.h"

#include <direct.h>

GameObject* cubeObject;

// A few includes needed to set up the console window
#if defined(DEBUG) || defined(_DEBUG)
#include <ios>
#include <io.h>
#include <fcntl.h>

#include "Gizmos.h"
#endif

#include "Font\Font.h"

// Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Default CTor for the engine
InfernoEngine::InfernoEngine()
{
	m_eEngineStatus = MANAGER_STATUS_UNINITALIZED;
	ZeroMemory(&m_msg, sizeof(MSG));

	m_sCanvasName = "StatisticsCanvas";
	m_pCanvas = nullptr;

	m_pFontManager = nullptr;
	m_pWorkerThreads = nullptr;

	m_uiFrameCount = 0;
	m_uiFrameRate = 0;

	Inferno::InitDumpFile();

#if defined(DEBUG) || defined(_DEBUG)
	// If we are in debug mode, create a console window for output
	// Quick check to see if we already have a console window
	if (GetConsoleWindow())
		return;

	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	AllocConsole();
	SetConsoleTitle(L"Debug Console");

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 1000;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// Redirect STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);

	// Redirect STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// Redirect STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
#endif
}

// Default DTor for the engine
InfernoEngine::~InfernoEngine()
{
	Destroy();
}

// IN: HINSTANCE - Instance of the application
//     LPCWSTR   - String holding the Icon resource ID  *OPTIONAL*
//     LPCWSTR   - String holding the SmallIcon resource ID  *OPTIONAL*
//     LPCWSTR   - String holding the name of the application  *OPTIONAL*
//     LPCWSTR   - String for the class name  *OPTIONAL*
// OUT: bool - States if the manager intialized correctly
//
// Initializes the engine and any magangers it will use
bool InfernoEngine::Initialize(HINSTANCE hInstance, LPCWSTR IconName, LPCWSTR SmallIconName, LPCWSTR className)
{
	if (m_eEngineStatus == MANAGER_STATUS_INITIALIZED || m_eEngineStatus == MANAGER_STATUS_ERROR)
		return false;

	PRINT("Initializing Engine...\n");

#if defined(DEBUG) || defined(_DEBUG)
	m_bDisplayGizmos = false;
#endif

	// Set up the audio manager before hand as it is needed for loading
	m_AudioManager.Initialize();
#if !defined(SHIPPING)
	m_AudioManager.SetBasePath(L"../InfernoEngine/Inc/Core/Public/WWiseResources/SoundBanks/");
#else
	m_AudioManager.SetBasePath(L"Data/Audio/");
#endif
	m_AudioManager.LoadSoundBank(L"Init.bnk");
	m_AudioManager.LoadSoundBank(L"BackgroundMusic.bnk");
	m_AudioManager.LoadSoundBank(L"SoundEffects.bnk");

	// Assign all pointers to prevent doing GetInstance calls
	m_pGraphicsManager = new GraphicsManager();
	m_pObjectManager = new ObjectManager();
	m_pPhysics = new Physics();
	m_pCameraManager = new CameraManager();
	m_pAssetManager = new AssetManager();
	m_pFontManager = new FontManager();
	m_pUIManager = new UIManager();
	m_pAIModule = new AIModule();
	m_pSceneManager = new SceneManager();
	m_pWorkerThreads = new WorkerThreadPool();

	// Load the config file before doing any intializations
	LoadConfig();

	m_pGraphicsManager->ApplyWindowChanges();

	// Call all Initalize functions of the managers
	if (!m_pGraphicsManager->Initialize(hInstance, this, IconName, SmallIconName, className) ||
		!InputManager::Initialize(m_pGraphicsManager->Window()) ||
		!m_pFontManager->Initialize(m_pGraphicsManager->Window(), m_pGraphicsManager->m_pDevice, m_pGraphicsManager->m_pSwapChain))
	{
		PRINT("FAILED to setup engine!\n");

		m_eEngineStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	Time::Initialize();

	m_pObjectManager->Initialize();

	//m_pAssetManager->LoadAssets();

	m_pCameraManager->Initialize();

	m_pSceneManager->Initialize();

	//m_AudioManager.SetWorldScale(.12f);

	m_pUIManager->Init();
	m_pUIManager->PostInit();

#if defined(DEBUG) || defined(_DEBUG)
	//Gizmos::Initialize();
	m_bDisplayFPS = true;
#else
	m_bDisplayFPS = false;
#endif
	timer.SetTime(1.0f);
	timer.UseTrueDelta(true);
	timer.Reset();

	UINT canvasID = -1;
	CreateNewUICanvas(m_sCanvasName, &m_pCanvas, &canvasID);

	SetUITargetResolution(1024, 768);

	m_pStatisticsBackground = new Texture();
	m_pStatisticsBackground->SetImage(m_pGraphicsManager->GetImage(L"Fill.dds"));

	UIElement* StatisticsElement = new UIElement();
	StatisticsElement->GetTransform()->SetAnchors(Anchors::ANCHOR_TOPLEFT);
	StatisticsElement->GetTransform()->SetBounds(Vector2(70, 20));
	StatisticsElement->GetTransform()->SetPosition(35, 10);
	UIRenderer* image = StatisticsElement->AddComponent<UIRenderer>();
	image->SetTexture(m_pStatisticsBackground);
	image->SetColor(Color(0.1f, 0.1f, 0.1f, 0.5f));
	image->SetLayer(900);
	UIText* text = StatisticsElement->AddComponent<UIText>();
	text->SetLayer(902);
	text->SetText("FPS:");

	AddToView(StatisticsElement, m_sCanvasName);

	UIElement* StatistValuesElement = new UIElement();
	StatistValuesElement->GetTransform()->SetAnchors(Anchors::ANCHOR_TOPLEFT);
	StatistValuesElement->GetTransform()->SetBounds(Vector2(50, 20));
	StatistValuesElement->GetTransform()->SetPosition(45, 10);
	m_pFPSText = StatistValuesElement->AddComponent<UIText>();
	m_pFPSText->SetText("0");
	m_pFPSText->SetAlignment(TextAlignment::TRAILING);
	m_pFPSText->SetLayer(902);
	m_pFPSText->SetColor(Color(1, 1, 1, 1));

	AddToView(StatistValuesElement, m_sCanvasName);

	m_pCanvas = Inferno_UI::GetCanvasByName(m_sCanvasName);
	m_pCanvas->GetUIElement()->SetActive(m_bDisplayFPS);

	InputManager::RegisterKey(Key::Control);
	InputManager::RegisterKey(Key::R);

	//m_pGraphicsManager->PostInit();

	// === JACK TESTING
	InputManager::RegisterBinding("Mouse", Button::LeftClick);

	PRINT("Engine initialized Successfully!\n");

	m_eEngineStatus = MANAGER_STATUS_INITIALIZED;
	return true;
}

// IN: void
// OUT: bool - States if the engine destroyed successfully
//
// Destroys the engine and any manager it uses
bool InfernoEngine::Destroy()
{
	if (m_eEngineStatus == MANAGER_STATUS_UNINITALIZED || m_eEngineStatus == MANAGER_STATUS_DESTORYED)
		return false;

	PRINT("Cleaning up Engine...\n");

	DestroyCanvas(m_sCanvasName);

	InputManager::UnregisterKey(Key::Control);
	InputManager::UnregisterKey(Key::R);

	// Call destory on all of the systems the engine used
	if (m_pObjectManager)
		m_pObjectManager->Shutdown();

	// Delete the Object Manager before destroying the graphics manager to ensure that all
	// of our objects can unregister properly.
	SAFE_DELETE(m_pObjectManager);

	if (m_pGraphicsManager)
		m_pGraphicsManager->Destroy();

	if (m_pFontManager)
		m_pFontManager->Destroy();

	m_AudioManager.UnloadSoundBank(L"Init.bnk");
	m_AudioManager.UnloadSoundBank(L"BackgroundMusic.bnk");
	m_AudioManager.UnloadSoundBank(L"SoundEffects.bnk");
	m_AudioManager.Shutdown();

	m_pWorkerThreads->Shutdown();

	// Delete the instance for all of the systems
	SAFE_DELETE(m_pUIManager);
	SAFE_DELETE(m_pGraphicsManager);
	SAFE_DELETE(m_pPhysics);
	SAFE_DELETE(m_pCameraManager);
	SAFE_DELETE(m_pAssetManager);
	SAFE_DELETE(m_pFontManager);
	SAFE_DELETE(m_pAIModule);
	SAFE_DELETE(m_pSceneManager);
	SAFE_DELETE(m_pWorkerThreads);

	EventSystem::GetInstance()->Shutdown();
	EventSystem::DeleteInstance();

	SAFE_DELETE(m_pStatisticsBackground);

	PRINT("Engine clean up complete!\n");
	m_eEngineStatus = MANAGER_STATUS_DESTORYED;
	return true;
}

// IN: void
// OUT: void
//
// Informs the engine we want to quit
void InfernoEngine::Quit()
{
	ClearScene();
	ClearView();
	CleanUpParticles();
	
	m_eEngineStatus = MANAGER_STATUS_QUIT;
}

// IN: void
// OUT: void
//
// Loads from the config file
void InfernoEngine::LoadConfig()
{
	FileSystem fs;
	bool configExists = true;

	// Try to open the config file
	std::fstream config;
	config.open(fs.GetDataFolder() + "config.txt", std::ios_base::in);

	// If the config file doesn't exist, attempt to open the default config file
	if (!config.is_open())
	{
		configExists = false;
		config.open("defaultconfig.txt", std::ios_base::in);

		if (!config.is_open())
		{
			ALERT("Default Config file is missing!");
			return;
		}
	}

	char previousLine[200];

	// Loop through our file
	while (!config.eof())
	{
		config.getline(previousLine, 200, '\n');

		// Attempt to split the string using = as a dilimiter
		std::vector<std::string> splitString = Inferno::StringParse(previousLine, '=');

		// If the string didn't split, we are either at a blank line or a group header
		if (splitString.size() < 2)
			continue;

		// Audio Values
		// --------------------------------------------------------------------------------
		if (splitString[0] == "fMasterVolume")
			m_AudioManager.SetMasterVolume((float)atof(splitString[1].c_str()));
		else if (splitString[0] == "fMusicVolume")
			m_AudioManager.SetBackgroundMusicVolume((float)atof(splitString[1].c_str()));
		else if (splitString[0] == "fSFXVolume")
			m_AudioManager.SetSFXVolume((float)atof(splitString[1].c_str()));

		// Graphics Values
		// --------------------------------------------------------------------------------
		else if (splitString[0] == "fWidth")
			m_pGraphicsManager->m_fWidth = (float)atof(splitString[1].c_str());
		else if (splitString[0] == "fHeight")
			m_pGraphicsManager->m_fHeight = (float)atof(splitString[1].c_str());
		else if (splitString[0] == "uWindowMode")
		{
			int styleVal = atoi(splitString[1].c_str());

			if (styleVal < 0 || styleVal > 2)
				m_pGraphicsManager->SetWindowStyle(WINDOW_STYLE_WINDOWED);
			else
				m_pGraphicsManager->SetWindowStyle((WINDOW_STYLE)styleVal);
		}
		else if (splitString[0] == "bVsyncEnabled")
			m_pGraphicsManager->SetVSync(Inferno::to_bool(atoi(splitString[1].c_str())));
		else if (splitString[0] == "uAnisotropic")
			m_pGraphicsManager->SetAnisotropicFiltering(atoi(splitString[1].c_str()));
		else if (splitString[0] == "fBrightness")
			m_pGraphicsManager->SetGamma((float)atof(splitString[1].c_str()));

#if defined(DEBUG) || defined(_DEBUG)
		// Debug Values
		// --------------------------------------------------------------------------------
		else if (splitString[0] == "bGizmos")
			m_bDisplayGizmos = Inferno::to_bool(atoi(splitString[1].c_str()));
		else if (splitString[0] == "bUseUncappedDelta")
			Time::s_bUseUncappedDelta = Inferno::to_bool(atoi(splitString[1].c_str()));
#endif
	}

	config.close();

	if (!configExists)
	{
		if (!fs.DirectoryExistsW(fs.GetDataFolderW()))
		{
			std::string MyGamesPath = Inferno::GetEnvironmentVariablePath("USERPROFILE") + "\\Documents\\My Games\\";
			if (!fs.DirectoryExists(MyGamesPath))
				_mkdir(MyGamesPath.c_str());

			_wmkdir(fs.GetDataFolderW().c_str());
		}

		std::string filepath = fs.GetDataFolder() + "config.txt";
		config.open(filepath.c_str(), std::ios_base::out);

		if (!config.is_open())
		{
			ALERT("Failed to create a config file!");
			return;
		}

		config << "[Audio]\n";
		config << "fMasterVolume=" << "0.12" << '\n';
		config << "fMusicVolume=" << "1.0" << '\n';
		config << "fSFXVolume=" << "1.0" << '\n';
		config << "\n[Graphics]\n";
		config << "fWidth=" << m_pGraphicsManager->GetWindowWidth() << '\n';
		config << "fHeight=" << m_pGraphicsManager->GetWindowHeight() << '\n';
		config << "uWindowMode=" << (int)m_pGraphicsManager->GetWindowStyle() << '\n';
		config << "bVsyncEnabled=" << (m_pGraphicsManager->VSyncEnabled() ? 1 : 0) << '\n';
		config << "uAnisotropic=" << m_pGraphicsManager->GetAnisotropicFiltering() << '\n';
		config.close();
	}
}

// IN: vector<pair<string, string>> - First string is name to update, Second string is value to replace
// OUT: void
//
// Updates the config file parameters that are requested with the updated values
void InfernoEngine::UpdateConfigFile(std::vector<std::pair<std::string, std::string>>& parameters)
{
	FileSystem fs;
	std::string oldConfigFilePath = fs.GetDataFolder() + "config.txt";
	std::string newConfigFilePath = fs.GetDataFolder() + "newConfig.txt";

	m_mConfigMutex.lock();

	std::fstream config;
	config.open(oldConfigFilePath, std::ios_base::in);

	if (config.is_open())
	{
		std::fstream newConfig;
		newConfig.open(newConfigFilePath, std::ios_base::out);

		if (newConfig.is_open())
		{
			std::string source;

			while (!config.eof())
			{
				std::getline(config, source);

				auto iter = parameters.begin();
				for (; iter < parameters.end(); ++iter)
				{
					if (source.find(iter->first) != std::string::npos)
					{
						source = iter->first + "=" + iter->second;
						parameters.erase(iter);
						break;
					}
				}

				newConfig << source;

				// This is needed as the newlines get lost from reading in to a string, but if we don't check for eof it will
				// add an extra new line each time this function is called. This prevents that from happening and preserves the
				// original newline count
				if (!config.eof())
					newConfig << '\n';
			}

			newConfig.close();
			config.close();

			std::remove(oldConfigFilePath.c_str());
			std::rename(newConfigFilePath.c_str(), oldConfigFilePath.c_str());
		}
		else
		{
			config.close();
			ALERT("Was unable to create a new config file!");
		}
	}
	else
	{
		ALERT("Was unable to open the config file!");
	}

	m_mConfigMutex.unlock();
}

// IN: void
// OUT: bool - States if the engine updated successfully
//
// Updates only a few components during primary asset loading
bool InfernoEngine::UpdatePreAsset()
{
	if (m_eEngineStatus != MANAGER_STATUS_INITIALIZED || m_msg.message == WM_QUIT)
		return false;

	// Update our time to get a proper delta from last frame
	Time::Update();

	//Updates sound manager to continue or kill sounds
	//m_AudioManager.Update();

	// Update our events
	EventSystem::GetInstance()->Update();

	// Upudate all our game objects based on the changes made from physics
	m_pObjectManager->Update();

	if (Time::s_bFixedUpdate)
	{
		m_pObjectManager->FixedUpdate();
		m_pObjectManager->LateUpdate();
	}

	// Update our inputs so we can get proper frame events when they happen
	if (!InputManager::Update())
	{
		m_eEngineStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	// Update our CameraManager
	m_pCameraManager->Update();

	// Do a quick check to see if we are trying to turn on or off the FPS display
	if (InputManager::IsKeyDown(Key::Control) && InputManager::IsKeyDown(Key::R))
	{
		if (InputManager::IsKeyPressed(Key::Control) || InputManager::IsKeyPressed(Key::R))
		{
			m_bDisplayFPS = !m_bDisplayFPS;
			m_pCanvas->GetUIElement()->SetActive(m_bDisplayFPS);
		}
	}

	m_uiFrameCount++;
	if (timer.IsFinished())
	{
		m_uiFrameRate = m_uiFrameCount;
		m_uiFrameCount = 0;
		timer.Reset();

		if (m_bDisplayFPS)
		{
			m_pFPSText->SetText(std::to_string(m_uiFrameRate));
		}
	}

	m_pUIManager->Update();

	// Update all our graphics now that our objects are in their proper places
	if (!m_pGraphicsManager->Update())
	{
		m_eEngineStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	while (PeekMessage(&m_msg, m_pGraphicsManager->m_pWindow->GetHWnd(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}

	return true;
}

// IN: void
// OUT: bool - States if the engine updated successfully
//
// Updates all components of the engine
bool InfernoEngine::Update()
{
	if (m_eEngineStatus != MANAGER_STATUS_INITIALIZED || m_msg.message == WM_QUIT)
		return false;

	// Update our time to get a proper delta from last frame
	Time::Update();

	//Updates sound manager to continue or kill sounds
	m_AudioManager.Update();

	// Update the AI_Module
	m_pAIModule->Update();

	// Update Phsyics First. This allows us to also handle all Collision and Trigger Events on the frames they happen on the component side
	if (Time::s_bFixedUpdate)
		m_pPhysics->FixedUpdate();

	// Update our events
	EventSystem::GetInstance()->Update();

	m_pUIManager->Update();

	// Upudate all our game objects based on the changes made from physics
	m_pObjectManager->Update();

	if (Time::s_bFixedUpdate)
	{
		m_pObjectManager->FixedUpdate();
		m_pObjectManager->LateUpdate();
	}

	// Update our inputs so we can get proper frame events when they happen
	if (!InputManager::Update())
	{
		m_eEngineStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	// Update our CameraManager
	m_pCameraManager->Update();

	// Do a quick check to see if we are trying to turn on or off the FPS display
	if (InputManager::IsKeyDown(Key::Control) && InputManager::IsKeyDown(Key::R))
	{
		if (InputManager::IsKeyPressed(Key::Control) || InputManager::IsKeyPressed(Key::R))
		{
			m_bDisplayFPS = !m_bDisplayFPS;
			m_pCanvas->GetUIElement()->SetActive(m_bDisplayFPS);
		}
	}

	m_uiFrameCount++;
	if (timer.IsFinished())
	{
		m_uiFrameRate = m_uiFrameCount;
		m_uiFrameCount = 0;
		timer.Reset();

		if (m_bDisplayFPS)
		{
			m_pFPSText->SetText(std::to_string(m_uiFrameRate));
		}
	}

	// Update all our graphics now that our objects are in their proper places
	if (!m_pGraphicsManager->Update())
	{
		m_eEngineStatus = MANAGER_STATUS_ERROR;
		return false;
	}

#if defined(DEBUG) || defined(_DEBUG)
	if (m_bDisplayGizmos)
	{
		StartEvent(L"Gizmos");
		Gizmos::StartDraw();
		//m_pObjectManager->OnDrawGizmos();
		m_pAIModule->OnGizmoDraw();
		//m_pPhysics->OnGizmosDraw();
		EndEvent();
	}
#endif

	while (PeekMessage(&m_msg, m_pGraphicsManager->m_pWindow->GetHWnd(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}

	return true;
}

// IN: void
// OUT: float - the aspect ratio
//
// Returns the current aspect ratio
float InfernoEngine::GetAspectRatio() const
{
	return m_pGraphicsManager->GetAspectRatio();
}

// IN: void
// OUT: void
//
// Updates the window if any of the window settings changed
void InfernoEngine::WindowUpdate()
{
	m_pGraphicsManager->WindowUpdate();
}
// IN: void
// OUT: void
//
// Accept changes that have been made to the window
void InfernoEngine::ApplyWindowChanges()
{
	m_pGraphicsManager->ApplyWindowChanges();
}
// IN: void
// OUT: void
//
// Revert changes made to the board if ApplyWindowChanges hasn't been called
void InfernoEngine::RevertWindowChanges()
{
	m_pGraphicsManager->RevertWindowChanges();
}

// IN: wchar_t* - name of the texture to load
// OUT: unsigned int - index into the texture array
//
// Loads a texture if it hasn't been loaded yet and returns the index into the textures array
int InfernoEngine::LoadTexture(const wchar_t* filename)
{
	if (m_pGraphicsManager != nullptr)
		return m_pGraphicsManager->LoadTexture(filename);

	ALERT("GraphicsManager not created!\n");
	return 0;
}
// IN: wchar_t* - Name of hte texture to store in memory
// OUT: void
//
// Loads the texture file into memory to be loaded onto the GPU at a later point. This automatically
// queues the texture file for loading and should be used by Threads.
void InfernoEngine::StoreTextureInMemory(const wchar_t* filename)
{
	m_pGraphicsManager->StoreTextureInMemory(filename);
}

// IN: char* - Name of scene to load
// OUT: void
//
// Informs the SceneManager that we would like to change scenes when possible
void InfernoEngine::SceneChange(const char* sceneName)
{
	m_pSceneManager->LoadScene(std::string(sceneName));
}

// IN: Mesh* - Mesh to load
// OUT: unsigned int - starting index of the mesh
//
// Loads the passed in mesh into the graphics card if it hasn't been loaded yet and returns the first index location
unsigned int InfernoEngine::LoadMesh(Mesh* mesh)
{
	if (mesh == nullptr)
		return 0;

	for (size_t sMesh = 0; sMesh < mesh->GetSubMeshes().size(); sMesh++)
	{
		SubMesh &subMesh = mesh->GetSubMeshes()[sMesh];
		subMesh.startVertex = m_pGraphicsManager->LoadVerts(&subMesh.vertices[0], subMesh.vertices.size());
		subMesh.startIndex = m_pGraphicsManager->LoadIndices(&subMesh.indices[0], subMesh.indices.size());
	}

	return 0;
}
// IN: void* - Mesh to queue for loading
//     bool  - If the mesh should be finalized, in some cases we don't want to as with NavMeshes
// OUT: void
//
// Adds a mesh to the queue to be loaded. This should be used by seperate Threads.
void InfernoEngine::QueueLoadMesh(void* mesh, bool finalize)
{
	if (mesh == nullptr)
		return;

	m_pGraphicsManager->QueueLoadMesh(mesh, finalize);
}

void InfernoEngine::AssetLoader()
{
	m_pAssetManager->LoadAssets();
	m_pAssetManager->LoadPrefabs();
}

void InfernoEngine::LoadingFinished()
{
	m_pCameraManager->PostInit();
}

void* InfernoEngine::LoadAssets(std::future<bool>* future)
{
	WorkerThreadFuctionPointers* assetLoader = new WorkerThreadFuctionPointers();
	assetLoader->WorkFunction = std::bind(&InfernoEngine::AssetLoader, this);
	assetLoader->CallbackFunction = std::bind(&InfernoEngine::LoadingFinished, this);

	if (future)
		(*future) = assetLoader->Promise.get_future();

	m_pWorkerThreads->PostWork(assetLoader);

	return assetLoader;
}

// IN: wstring - name of the image
//     void*   - pointer to a Shader Resource View
// OUT: int - ID of the Image added
//
// Adds the shader resource view to the active list of images
int InfernoEngine::AddImage(std::wstring name, void* image)
{
	return m_pGraphicsManager->AddImage(name, image);
}

// IN: wstring  - Name for the ShaderResource
//     wstring* - Pointer to the array of image names
//     uint     - number of image names
// OUT: int - ID to the ShaderResourceView
int InfernoEngine::CreateTextureArray(std::wstring name, std::wstring* textureNames, UINT numTextures)
{
	return m_pGraphicsManager->CreateTextureArray(name, textureNames, numTextures);
}

// IN: void
// OUT: void
//
// Resets the Time class
void InfernoEngine::ResetTime()
{
	Time::Reset();
}

void InfernoEngine::PostAudioEvent(unsigned int aEvent)
{
	m_AudioManager.PostEvent(aEvent);

}

void InfernoEngine::StopAllAudio()
{
	m_AudioManager.StopAllAudio();
}

void InfernoEngine::RegisterObjectToAudioManager(GameObject * _object)
{
	m_AudioManager.RegisterEntity(_object, _object->GetName().c_str());
}

void InfernoEngine::UnregisterObjectToAudioManager(GameObject * _object)
{
	m_AudioManager.UnRegisterEntity(_object);
}

void InfernoEngine::PostAudioEventAtObjectPosition(unsigned int aEvent, GameObject * _object)
{
	m_AudioManager.PostEvent(aEvent, _object->transform->GetPosition());
}

void InfernoEngine::SetMasterVolume(float volume)
{
	m_AudioManager.SetMasterVolume(volume);
}

void InfernoEngine::SetSFXVolume(float volume)
{
	m_AudioManager.SetSFXVolume(volume);
}

void InfernoEngine::SetBackgroundMusicVolume(float volume)
{
	m_AudioManager.SetBackgroundMusicVolume(volume);
}

float InfernoEngine::GetMasterVolume()
{
	return m_AudioManager.GetMasterVolume();
}

float InfernoEngine::GetBackgroundVolume()
{
	return m_AudioManager.GetBackgroundVolume();
}

float InfernoEngine::GetSFXVolume()
{
	return m_AudioManager.GetSFXVolume();
}

// IN: GameObject - Object to copy
//     Vector3    - Position of object
//     Quaternion - Rotation of object
//     Vector3    - Scale of object
// OUT: GameObject* - pointer to the newly created object
//
// Creates an object based on the incoming GameObject
GameObject* InfernoEngine::Instantiate(GameObject* object, Vector3 position, Quaternion rotation, Vector3 Scale)
{
	return nullptr;
}

// IN : NavAgent* - Pointer to the NavAgent to register
// OUT : bool - Success in registering the NavAgent
//
// Registers a NavAgent with the AI Module
bool InfernoEngine::RegisterNavAgent(NavAgent* _navAgent)
{
	return m_pAIModule->RegisterAgent(_navAgent);
}

// IN : AIEntity* - The entity to register
// OUT : bool - The result
//
// Returns true if the register is success
bool InfernoEngine::RegisterAIEntity(AIEntity* _entity)
{
	return m_pAIModule->RegisterAIEntity(_entity);
}

// IN: GameObject* - Pointer to the Player
// OUT: bool - True if register is success
//
// Registers the player object to the AI Module
bool InfernoEngine::RegisterPlayer(GameObject* _player)
{
	return m_pAIModule->RegisterPlayer(_player);
}

// IN : Mesh* - Pointer to the navMesh
// OUT : bool - Success
//
// Initializes the AIModules NavMesh
bool InfernoEngine::RegisterNavMesh(Mesh* _mesh, Matrix4& _globalPosition)
{
	return m_pAIModule->LoadNavMesh(_mesh, _globalPosition);
}

// IN: Renderer* - Pointer to the renderer to register
// OUT: bool - Success in registering the renderer
//
// Registers a renderer for rendering with the Graphics Manager
bool InfernoEngine::RegisterRenderComponent(Renderer* _rComponent)
{
	m_pGraphicsManager->RegisterRenderer(_rComponent);

	return true;
}

// IN : UIRenderer* - Pointer to the UIRenderer
// OUT : bool - Success in registering the component
//
// Registers a UIRenderer with the Graphics Manager
bool InfernoEngine::RegisterUIRenderComponent(UIRenderer* _rComponent)
{
	m_pGraphicsManager->RegisterUIRenderer(_rComponent);

	return true;
}

//  IN : Light* - Pointer to the Light Component that needs to be registered
//  OUT : bool - Whether or not the lComponent was able to be registered
// 
//  Registers the given Light with the Light Manager. Returns true if it was registered, false otherwise.
bool InfernoEngine::RegisterLightComponent(Light* _lComponent)
{
	return m_pGraphicsManager->RegisterLightComponent(_lComponent);
}

// IN: ParticleSystem* - Pointer to the Particle System Component that needs to be registered
// OUT: bool - Whether or not the Component was able to be registered
//
// Registers the given Particle System with the Particle Manager.
bool InfernoEngine::RegisterParticleSystem(ParticleSystem* _pComponent)
{
	return m_pGraphicsManager->RegisterParticleSystem(_pComponent);
}

// IN: Rigidbody* - Pointer to the RigidBody to register
// OUT: bool - Success in registering the RigidBody
//
// Registers a RigidBody with the Physics Module
bool InfernoEngine::RegisterRigidBody(Rigidbody* rigidBody)
{
	m_pPhysics->AddRigidbody(rigidBody);
	return true;
}

// IN: Collider* - Pointer to the Collider to register
// OUT: bool - Success in registering the Collider
//
// Registers a Collider with the Physics Module
bool InfernoEngine::RegisterCollider(Collider* collider)
{
	m_pPhysics->AddCollider(collider);
	return true;
}

// IN: UIEventSystem* - Pointer to the UIEventSystem to register
// OUT: bool - Success in registering the UIEventSystem
//
// Registers a UIEventSystem with the UIManager
bool InfernoEngine::RegisterUIEventSystem(UIEventSystem* eventSystem)
{
	if (m_pUIManager == nullptr)
		return false;

	m_pUIManager->RegisterObject(eventSystem);
	return true;
}

// IN: GameObject*
// OUT: void
//
// Registers the object as the audio listener
void InfernoEngine::RegisterAudioListener(const GameObject* gameObject)
{
	m_AudioManager.RegisterListener(gameObject, "MainCamera");
}

//  IN :  void
//  OUT : void
//
// Tells the AIModule that the game is in a cutscene
void InfernoEngine::EnterCutscene()
{
	m_pAIModule->EnterCutscene();
}

//  IN :  void
//  OUT : void
//
// Tells the AIModule that the game is in a cutscene
void InfernoEngine::ExitCutscene()
{
	m_pAIModule->ExitCutscene();
}

//  IN : NavAgent* - Pointer to the NavAgent to unregister
//  OUT : bool - Whether or not it was unregistered
//
//  Unregisters the given NavAgent from the AI Module. Returns true if it was found and unregistered, false otherwise.
bool InfernoEngine::UnregisterNavAgent(NavAgent* _navAgent)
{
	return m_pAIModule->UnregisterAgent(_navAgent);
}

// IN : AIEntity* - The entity to unregister
// OUT : bool - The result
//
// Returns true if the register is success
bool InfernoEngine::UnregisterAIEntity(AIEntity* _entity)
{
	return m_pAIModule->UnregisterAIEntity(_entity);
}

// IN : GameObject* - Pointer to the player object
// OUT : bool - True if unregistered success
//
// Unregisters the player object from the AI Module
bool InfernoEngine::UnregisterPlayer(GameObject* _player)
{
	return m_pAIModule->UnregisterPlayer(_player);
}

//  IN : Renderer* - The pointer to the Renderer Component to unregister
//  OUT : bool - Whether or not it was unregistered
//
//  Unregisters the given Renderer from the Graphics Manager. Returns true if it was found and unregistered, false otherwise.
bool InfernoEngine::UnregisterRenderComponent(Renderer* _rComponent)
{
	m_pGraphicsManager->UnregisterRenderer(_rComponent);

	return true;
}

//  IN : UIRenderer* - The pointer to the UIRenderer Component to unregister
//  OUT : bool - Whether or not it was unregistered
//
//  Unregisters the given UIRenderer from the Graphics Manager. Returns true if it was found and unregistered, false otherwise.
bool InfernoEngine::UnregisterUIRenderComponent(UIRenderer* _rComponent)
{
	m_pGraphicsManager->UnregisterUIRenderer(_rComponent);

	return true;
}

//  IN : Light* - Pointer to the Light Component to unregistered
//  OUT : bool - Whether or not it was unregistered
// 
//  Unregisters the given Light with the Light Manager. Returns true if it was registered, false otherwise.
bool InfernoEngine::UnregisterLightComponent(Light* _lComponent)
{
	return m_pGraphicsManager->UnregisterLightComponent(_lComponent);
}

// IN: ParticleSystem* - Pointer to the Particle System Component to unregister
// OUT: bool - Whether or not the Component was unregistered
//
// Uregisters the given Particle System with the Particle Manager.
bool InfernoEngine::UnregisterParticleSystem(ParticleSystem* _pComponent)
{
	return m_pGraphicsManager->UnregisterParticleSystem(_pComponent);
}

//  IN : Collider* - The pointer to the Collider to unregister
//  OUT : bool - Whether or not it was unregistered
//
//  Unregisters the given Collider from the Physics Module. Returns true if it was found and unregistered, false otherwise.
bool InfernoEngine::UnregisterColliderComponent(Collider* _collider)
{
	m_pPhysics->RemoveCollider(_collider);
	return true;
}

//  IN : Rigidbody* - The pointer to the Rigidbody to unregister
//  OUT : bool - Whether or not it was unregistered
//
//  Unregisters the given Rigidbody from the Physics Module. Returns true if it was found and unregistered, false otherwise.
bool InfernoEngine::UnregisterRigidbodyComponent(Rigidbody* _rigidbody)
{
	m_pPhysics->RemoveRigidbody(_rigidbody);
	return true;
}

//  IN : UIEventSystem - The pointer to the UIEventSystem to unregister
//  OUT : Whether or not it was unregistered
//
//  Unregisters the given UIEventSystem from the UIManager. Returns true if it was found and unregistered, false otherwise.
bool InfernoEngine::UnregisterUIEventSystem(UIEventSystem* _eventSystem)
{
	if (m_pUIManager == nullptr)
		return false;

	m_pUIManager->UnregisterEventSystem(_eventSystem);
	return true;
}

// IN: void
// OUT: void
//
// Applies the current pipeline state
void InfernoEngine::ApplyPipelineState()
{
	m_pGraphicsManager->ApplyPipelineState();
}

// IN: char* - Name of the effect
//     int   - ID of the pixel shader
//     int   - ID of the rasterizer state
//     int   - ID of the blend state
//     int*  - List of SamplerState IDs
//     int   - Count of the sampler states
// OUT: int - ID of the added effect
//
// Creates an effect and adds it to the list of effects if it doesn't already exits and returns the ID
int InfernoEngine::CreateEffect(const char* name, int pixelShader, int rasterizerState, int blendState, int* samplerStates, int samplerCount)
{
	if (m_pGraphicsManager)
		return m_pGraphicsManager->CreateEffect(name, pixelShader, rasterizerState, blendState, samplerStates, samplerCount);

	return -1;
}

// IN: int                  - ID of the shader resource view
//     UINT                 - Size of each element in bytes
//     UINT                 - Flags to apply to the UAV
// OUT: int - ID of the created UAV, -1 on error
//
// Creates an unordered access view for the requested shader resource view
int InfernoEngine::CreateUnorderedAccessView(int textureID, UINT Flags, UINT format)
{
	return m_pGraphicsManager->CreateUnorderedAccessView(textureID, Flags, format);
}

// IN: wstring - Name of the texture
//     UINT    - Width of the texture
//     UINT    - Height of the texture
//     UINT    - Size of the border, 0 for none
//     Color   - Color of the background
//     Color   - Color of the boarder if border size is greater then 0
// OUT: int - ID of the image that was created
//
// Creates a solid color image with a border if one is requested
int InfernoEngine::CreateSolidColorImage(std::wstring name, UINT width, UINT height, UINT borderSize, Color backColor, Color borderColor)
{
	return m_pGraphicsManager->CreateSolidColorImage(name, width, height, borderSize, backColor, borderColor);
}

// IN: Mesh* - pointer to the mesh requesting verts
// OUT: vector<unsigned int> - List of offsets, size 0 on error
//
// Evaluates the mesh and available skinning verts and returns a list of offsets for writing
// each submesh to the skinning vertex buffer
std::vector<unsigned int> InfernoEngine::RequestSkinningVerts(Mesh* mesh)
{
	return m_pGraphicsManager->RequestSkinningVerts(mesh);
}
// IN: Mesh*        - pointer to the mesh releasing verts
//     unsigned int - first offset of the lsit
// OUT: void
//
// Releases the vertices the mesh is writing so other meshes may use them
void InfernoEngine::ReleaseSkinningVerts(Mesh* mesh, unsigned int firstOffset)
{
	m_pGraphicsManager->ReleaseSkinningVerts(mesh, firstOffset);
}

// IN: void
// OUT: GameObject* - new game object
//
// Returns a new game object to use
GameObject* InfernoEngine::GetNewGameObject()
{
	return m_pObjectManager->Instantiate();
}

// IN: char*  - name of the mesh to grab
// OUT: Mesh* - pointer to the mesh
//
// Returns the requested mesh
Mesh* InfernoEngine::GetMeshByName(const char* name)
{
	return m_pAssetManager->GetMesh(std::string(name));
}

// IN: string  - name of the material to grab
// OUT: Material* - pointer to the mesh
//
// Returns the requested material
Material* InfernoEngine::GetMaterial(std::string& _materialName){
	return m_pAssetManager->GetMaterial(_materialName);
}

// IN: ShaderType - type of shader to grab
//     wstring    - name of shader
// OUT: int - ID of shader, -1 on error
//
// Returns the ID of requested shader if it exists
int InfernoEngine::GetShader(ShaderType type, std::wstring name)
{
	return m_pGraphicsManager->GetShader(type, name);
}
// IN: wstring - name of buffer
// OUT: int - ID of buffer, -1 on error
//
// Returns the ID of requested buffer if it exists
int InfernoEngine::GetConstantBuffer(std::wstring name)
{
	return m_pGraphicsManager->GetConstantBuffer(name);
}
// IN: BufferType - type of buffer
//     wstring    - name of buffer
// OUT: int - ID of buffer, -1 on error
//
// Returns the ID of requested buffer if it exists
int InfernoEngine::GetBuffer(BufferType type, std::wstring name)
{
	return m_pGraphicsManager->GetBuffer(type, name);
}
// IN: wstring - name of image
// OUT: int - ID of image, -1 on error
//
// Returns the ID of requested image if it exists
int InfernoEngine::GetImage(std::wstring name)
{
	return m_pGraphicsManager->GetImage(name);
}
// IN: SHADER_RESOURCE_TYPE - type of resource to get
//     wstring              - name of Shader Resource
// OUT: int - ID of Shader Resource
//
// Returns the requested shader resource ID
int InfernoEngine::GetShaderResource(SHADER_RESOURCE_TYPE type, std::wstring name)
{
	return m_pGraphicsManager->GetShaderResource(type, name);
}
// IN: wstring - name of Unordered Access View
// OUT: int - ID of Unordered Access View
//
// Returns the requested Unordered Access View ID
int InfernoEngine::GetUnorderedAccessView(std::wstring name)
{
	return m_pGraphicsManager->GetUnorderedAccessView(name);
}
// IN: wstring - name of input layout
// OUT: int - ID of input layout, -1 on error
//
// Returns the ID of requested input layout if it exists
int InfernoEngine::GetInputLayout(std::wstring name)
{
	return m_pGraphicsManager->GetInputLayout(name);
}
// IN: wstring - name of render target view
// OUT: int - ID of render target view, -1 on error
//
// Returns the ID of requested render target view if it exists
int InfernoEngine::GetRenderTargetView(std::wstring name)
{
	return m_pGraphicsManager->GetRenderTargetView(name);
}
// IN: wstring - name of depth stencil view
// OUT: int - ID of depth stencil view, -1 on error
//
// Returns the ID of requested depth stencil view if it exists
int InfernoEngine::GetDepthStencilView(std::wstring name)
{
	return m_pGraphicsManager->GetDepthStencilView(name);
}
// IN: wstring - name of sampler state
// OUT: int - ID of sampler state, -1 on error
//
// Returns the ID of requested sampler state if it exists
int InfernoEngine::GetSamplerState(std::wstring name)
{
	return m_pGraphicsManager->GetSamplerState(name);
}
// IN: wstring - name of rasterizer state
// OUT: int - ID of rasterizer state, -1 on error
//
// Returns the ID of requested rasterizer state if it exists
int InfernoEngine::GetRasterizerState(std::wstring name)
{
	return m_pGraphicsManager->GetRasterizerState(name);
}
// IN: wstring - name of blend state
// OUT: int - ID of the blend state, -1 on error
//
// Returns the ID of the requested blend state if it exists
int InfernoEngine::GetBlendState(std::wstring name)
{
	return m_pGraphicsManager->GetBlendState(name);
}
// IN: wstring - name of the depth stencil state
// OUT: int - ID of the depth stencil state, -1 on error
//
// Returns the ID of the requested depth stencil state if it exists
int InfernoEngine::GetDepthStencilState(std::wstring name)
{
	return m_pGraphicsManager->GetDepthStencilState(name);
}

// IN: char* - name of the effect
// OUT: int - ID of the effect
//
// Returns the ID of the requested effect
int InfernoEngine::GetEffectByName(const char* name)
{
	return m_pGraphicsManager->GetEffectByName(name);
}

// IN: char* - Name of the animation
// OUT: Animation* - Pointer to the aniation
//
// Returns the pointer to the requested animation
Animation* InfernoEngine::GetAnimation(const char* name)
{
	return m_pAssetManager->GetAnimation(std::string(name));
}

// IN: char* - Name of the animation controller
// OUT: Animation* - Pointer to the animation controller
//
// Returns the pointer to the requested animation controller
AnimationController* InfernoEngine::GetAnimationControllerByName(const char* name)
{
	return m_pAssetManager->GetAnimationController(std::string(name));
}

// IN: unsigned int - ID of the layout
// OUT: void
//
// Assigns the requested input layout to the InputAssembler
void InfernoEngine::SetInputLayout(unsigned int ID)
{
	m_pGraphicsManager->SetInputLayout(ID);
}
// IN: ShaderType - type of shader to apply
//     int        - ID to the shader being applied
// OUT: void
//
// Sets the current shader type at the passed in ID to the pipeline
void InfernoEngine::SetShader(ShaderType type, int ID)
{
	m_pGraphicsManager->SetShader(type, ID);
}
// IN: ShaderType   - Type of Shader to apply to
//     unsigned int - ID of the constant buffer
//     unsigned int - Slot of the buffer
// OUT: void
//
// Assigns the passed in constant buffer to the pipeline
void InfernoEngine::SetConstantBuffer(ShaderType type, unsigned int ID, unsigned int Slot)
{
	m_pGraphicsManager->SetConstantBuffer(type, ID, Slot);
}
// IN: unsigned int - ID of the buffer
//     unsigned int - stride of the buffer in bytes
//     unsigned int - offset of the buffer in bytes
// OUT: void
//
// Applies the passed in vertex buffer to the InputAssembler
void InfernoEngine::SetVertexBuffer(unsigned int ID, unsigned int stride, unsigned int offset)
{
	m_pGraphicsManager->SetVertexBuffer(ID, stride, offset);
}
// IN: unsigned int - ID of the buffer
// OUT: void
//
// Applies the passed in index buffer to the InputAssembler
void InfernoEngine::SetIndexBuffer(unsigned int ID)
{
	m_pGraphicsManager->SetIndexBuffer(ID);
}

// IN: Matrix4 - World Matrix to apply
// OUT: void
//
// Applies the passed in world matrix to the world buffer
void InfernoEngine::SetWorldMatrix(const Matrix4& world)
{
	m_pGraphicsManager->SetWorldMatrix(world);
}
// IN: Matrix4 - View Matrix to apply
// OUT: void
//
// Applies the passed in view matrix to the view buffer
void InfernoEngine::SetViewMatrix(const Matrix4& view)
{
	m_pGraphicsManager->SetViewMatrix(view);
}
// IN: Matrix4 - Projection Matrix to apply
// OUT: void
//
// Applies the passed in projection matrix to the projection buffer
void InfernoEngine::SetProjectionMatrix(const Matrix4& projection)
{
	m_pGraphicsManager->SetProjectionMatrix(projection);
}
// IN: COLOR - Color to apply
// OUT: void
//
// Applies the passed in color to the color buffer
void InfernoEngine::SetColor(const Color& color)
{
	m_pGraphicsManager->SetColor(color);
}
// IN: Vector2 - UV Offset to apply
// OUT: void
//
// Applies the passd in uv offsets to the offset buffer
void InfernoEngine::SetUVOffsets(const Vector2& offset, const Vector2& tiling)
{
	m_pGraphicsManager->SetUVOffsets(offset, tiling);
}
// IN: ShaderType - Shader to apply image to
//     int        - Image ID
//     int        - Slot to apply
// OUT: void
//
// Applies the requested image to the requested shader slot
void InfernoEngine::SetImage(ShaderType type, int ID, int Slot)
{
	m_pGraphicsManager->SetTexture(type, ID, Slot);
}
// IN: ShaderType - Shader to apply UAV to
//     int        - UAV ID
//     int        - Slot to apply
// OUT: void
//
// Applies a Unordered Access View to the requested slot of the requested shader type
void InfernoEngine::SetUnorderedAccessView(ShaderType type, int ID, int Slot, int Count)
{
	m_pGraphicsManager->SetUnorderedAccessView(type, ID, Slot, Count);
}

// IN: ShaderType - type of shader to apply to
//     int        - ID of the sampler to apply
//     int        - Slot for the sampler
// OUT: void
//
// Applies all the passed in states to the pipeline
void InfernoEngine::SetSamplerStates(ShaderType type, int ID, int Slot)
{
	m_pGraphicsManager->SetSamplerStates(type, ID, Slot);
}

// IN: TopologyType - Topology to set
// OUT: void
//
// Applies the requested topology
void InfernoEngine::SetTopology(TopologyType topology)
{
	m_pGraphicsManager->SetTopology(topology);
}

// IN: int - ID of Blend State
// OUT: void
//
// Updates the desired state with incoming information
void InfernoEngine::SetBlendState(int ID)
{
	m_pGraphicsManager->SetBlendState(ID);
}

// IN: int - ID of the Rasterizer State
// OUT: void
//
// Updates the desired state with the incoming information
void InfernoEngine::SetRasterizerState(int ID)
{
	m_pGraphicsManager->SetRasterizerState(ID);
}

// IN: int  - ID of the Depth Stencil State
//     UINT - Reference value for the stencil
// OUT: void
//
// Assigns the requested depth stencil state and ref value to the pipeline
void InfernoEngine::SetDepthStencilState(int ID, UINT ref)
{
	m_pGraphicsManager->SetDepthStencilState(ID, ref);
}

// IN: int - ID of the Depth Stencil View
// OUT: void
//
// Assigns the requested depth stencil view to the pipeline
void InfernoEngine::SetDepthStencilView(int ID)
{
	m_pGraphicsManager->SetDepthStencilView(ID);
}

// IN: UINT  - ID of buffer to update
//     void* - pointer to the data to update
//     UINT  - size of data in bytes
// OUT: void
//
// Updates the sub resource requested if the resource is a valid buffer
void InfernoEngine::UpdateSubResource(UINT ID, void* data, UINT size)
{
	m_pGraphicsManager->UpdateSubResource(ID, data, size);
}

// IN: int - ID of the buffer to copy to
//     int - offset to use
//     int - ID of the UVA that has a counter
// OUT: void
//
// Copies the invisible counter on the UAV to the buffer
void InfernoEngine::CopyStructureCount(int bufferID, int offset, int UAVID)
{
	m_pGraphicsManager->CopyStructureCount(bufferID, offset, UAVID);
}

// Draw calls that don't use the pipeline manager
// ------------------------------------------------------------------------------------------

// IN: unsigned int - Number of Vertices to render
//     unsigned int - Start Vertex
// OUT: void
//
// Draws the vertices with the current pipeline setup
void InfernoEngine::Draw(unsigned int numVertices, unsigned int startVertex)
{
	m_pGraphicsManager->Draw(numVertices, startVertex);
}

// IN: unsigned int - Number of Indices to render
//     unsigned int - Start Index
//     unsigned int - Start Vertex
// OUT: void
//
// Draws the indices using the ccurrent pipeline setup
void InfernoEngine::DrawIndexed(unsigned int numIndices, unsigned int startIndex, unsigned int startVertex)
{
	m_pGraphicsManager->DrawIndexed(numIndices, startIndex, startVertex);
}

//--------------------------------------------------------------------------------------------

// IN: int   - ID of the effect to use
//     Mesh* - Pointer to the mesh
// OUT: void
//
// Draws the mesh using the requested effect
void InfernoEngine::Draw(int EffectID, Mesh* mesh)
{
	m_pGraphicsManager->Draw(EffectID, mesh);
}
// IN: int - ID of the effect to use
//     int - Start Vertex
//     int - Num Vertices
// OUT: void
//
// Calls a standard draw call using the passed in effect ID
void InfernoEngine::Draw(int EffectID, int StartVertex, int NumVerts)
{
	m_pGraphicsManager->Draw(EffectID, StartVertex, NumVerts);
}
// IN: int          - ID of the effect to use
//     unsigned int - start index
//     unsinged int - num indices
//     unsigned int - start vertex
// OUT: void
//
// Draws the passed in mesh using the effect requested
void InfernoEngine::DrawIndexed(int EffectID, unsigned int startIndex, unsigned int numIndices, unsigned int startVertex)
{
	m_pGraphicsManager->DrawIndexed(EffectID, startIndex, numIndices, startVertex);
}
// IN: int  - ID of the buffer with the draw arguments
//     UINT - offset in bytes for the draw arguments
// OUT: void
//
// Applies changes to the pipeline and uses the buffer from the passed in ID to draw
void InfernoEngine::DrawIndirect(int BufferID, UINT offset)
{
	m_pGraphicsManager->DrawIndirect(BufferID, offset);
}
// IN: unsigned int - X thread groups
//     unsigned int - Y thread groups
//     unsigned int - Z thread groups
// OUT: void
//
// Calls dispatch on the current pipeline
void InfernoEngine::Disptach(unsigned int x, unsigned int y, unsigned int z)
{
	m_pGraphicsManager->Disptach(x, y, z);
}

void InfernoEngine::SetAmbientColor(float r, float g, float b)
{
	m_pGraphicsManager->SetAmbientColor(r, g, b);
}
void InfernoEngine::SetAmbientColor(Vector3 color)
{
	m_pGraphicsManager->SetAmbientColor(color);
}
void InfernoEngine::SetAmbientColor(Vector4 color)
{
	m_pGraphicsManager->SetAmbientColor(color);
}
void InfernoEngine::SetAmbientIntensity(float intensity)
{
	m_pGraphicsManager->SetAmbientIntensity(intensity);
}

// IN: float - The Target Width
//     float - The Target Height
// OUT: void
//
// Sets the target resolution that UI will be trying to position itself for.
void InfernoEngine::SetUITargetResolution(float _width, float _height)
{
	m_pUIManager->SetTargetResolution(_width, _height);
}

// IN: UICanvas* - { Out Parameter } Pointer to the Created UICanvas
//      unsigned int* - { Out Parameter } Pointer to the slot number that the created canvas was put in.
// OUT: void
//
// Creates a new UICanvas for UIElements to be added to. Returns the UICanvas, and the canvas slot through parameters.
void InfernoEngine::CreateNewUICanvas(std::string _canvasName, UICanvas** _createdCanvas, unsigned int* _canvasSlot)
{
	*_canvasSlot = m_pUIManager->CreateNewCanvas(_canvasName, _createdCanvas);
}

// IN: string - The name of the Canvas
// OUT: UICanvas* - The desired Canvas, if it exists
//
// Returns the Canvas found under the given name, if it exists.
UICanvas* InfernoEngine::GetCanvasByName(std::string _canvasName)
{
	return m_pUIManager->GetCanvasByName(_canvasName);
}

// IN: UIElement* - Pointer to the UIElement to add
//      unsigned int - The slot of the canvas to add the element to
// OUT: Bool - Whether or not the element was added
//
// Adds an UIElement to the desired Canvas, if that canvas is valid. Returns true if the element was added, false otherwise.
bool InfernoEngine::AddToView(UIElement* _element, unsigned int _canvasSlot)
{
	return m_pUIManager->AddToView(_element, _canvasSlot);
}

// IN: UIElement* - Pointer to the UIElement to add
//      string - The name of the desired Canvas
// OUT: Bool - Whether or not the element was added
//
// Adds an UIElement to the desired Canvas, if that canvas is valid. Returns true if the element was added, false otherwise.
bool InfernoEngine::AddToView(UIElement* _element, std::string _canvasName)
{
	return m_pUIManager->AddToView(_element, _canvasName);
}

// IN: UISelectable* - Pointer to the Selectable that should be focused on
// OUT: void
//
// Sets the UISelectable that the UIController should be focusing on
void InfernoEngine::SetCurrentSelectable(UISelectable* _selectable)
{
	m_pUIManager->SetCurrentSelectable(_selectable);
}

// IN: GameObject* - Pointer to the GameObject
// OUT: void
//
// Registers a GameObject with the ObjectManager's master list of GameObjects, which handles cleanup at the shutdown of the engine
void InfernoEngine::RegisterGameObject(GameObject* _gameObject)
{
	m_pObjectManager->RegisterObject(_gameObject);
}

#if defined(DEBUG) || defined(_DEBUG)
void InfernoEngine::StartEvent(const wchar_t* name)
{
	m_pGraphicsManager->StartEvent(name);
}
void InfernoEngine::EndEvent()
{
	m_pGraphicsManager->EndEvent();
}
#endif

// IN: GameObject* - Pointer to the GameObject
// OUT: void
// 
// Removes a GameObject from the ObjectManager's list of objects in the Scene. This doesn't mean that the object is being destroyed, just 
// stops the ObjectManager from calling that GameObjects update directly, this should happen when a GameObject gets parented to another, 
// as the parent handles calling the child's update.
void InfernoEngine::RemoveFromSceneList(GameObject* _gameObject)
{
	m_pObjectManager->RemoveFromSceneList(_gameObject);
}

// === Instaniate
//  IN : string - The name of the Prefab that you want back. Can be left blank if you want just a new GameObject.
//  OUT : GameObject* - Pointer to an instance of the Prefab you requested, or a blank GameObject.
//
//  Allocates and returns an instance of the Prefab that you request, if it exists. If it doesn't exist, or the parameter is left blank
//  then it just returns a new blank GameObject.
GameObject* InfernoEngine::Instantiate(std::string _prefabName)
{
	return m_pObjectManager->Instantiate(_prefabName);
}

// === Destroy
//  IN : GameObject* - Pointer to the GameObject that should be destroyed
//       float - (Optional) Delay time to destroy the GameObject
//  OUT : void
//
//  Removes 
void InfernoEngine::Destroy(GameObject* _gameObject, float _delayTime)
{
	m_pObjectManager->Destroy(_gameObject, _delayTime);
}

// IN: string - Name of the Canvas
// OUT: void
//
// Destroys a Canvas and all UIElements that are attached to the Canvas.
void InfernoEngine::DestroyCanvas(std::string _canvasName)
{
	m_pUIManager->DestroyCanvas(_canvasName);
}

bool InfernoEngine::AllocatePrefab(std::string _prefabName, GameObject* _basePrefab, unsigned int _amount)
{
	return m_pObjectManager->AllocatePrefab(_prefabName, _basePrefab, _amount);
}

void InfernoEngine::ClearScene()
{
	m_AudioManager.StopAllAudio();
	m_pObjectManager->RemoveAllGameObjects();
}

void InfernoEngine::UnregisterAudioListener(const GameObject* _gameObject)
{
	m_AudioManager.UnRegisterListener(_gameObject);
}

void InfernoEngine::TrueDestroy(GameObject* _gameObject)
{
	m_pObjectManager->TrueDestroy(_gameObject);
}

bool InfernoEngine::CreatePrefab(Prefab* _prefabData, unsigned int _allocationAmount){
	return m_pObjectManager->CreatePrefab(_prefabData, _allocationAmount);
}

IComponent* InfernoEngine::CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform){
	return m_pObjectManager->CreateComponent(_id, _gameObject, _transform);
}

UIElement* InfernoEngine::GetUIElementByName(std::string& _elementName, std::string& _canvasName)
{
	return m_pUIManager->GetUIElementByName(_elementName, _canvasName);
}

UISelectable* InfernoEngine::GetCurrentSelectable()
{
	return m_pUIManager->GetCurrentSelectable();
}

void InfernoEngine::AddForOnEnable(GameObject* _gameObject)
{
	m_pObjectManager->AddForOnEnable(_gameObject);
}

void InfernoEngine::RemoveFromOnEnable(GameObject* _gameObject)
{
	m_pObjectManager->RemoveFromOnEnable(_gameObject);
}

void InfernoEngine::AddForOnDisable(GameObject* _gameObject)
{
	m_pObjectManager->AddForOnDisable(_gameObject);
}

GameObject* InfernoEngine::GetGameObjectByName(std::string& _gameObjectName)
{
	return m_pObjectManager->GetGameObjectByName(_gameObjectName);
}

// IN: WorkerThread* - Pointer to work being added **NOTE** - This must be dynamic memory or locally stored till done
// OUT: void
//
// Adds work to the worker thread queue
void InfernoEngine::PushWork(WorkerThread* work)
{
	m_pWorkerThreads->PostWork(work);
}

// IN: void
// OUT: void
//
// Destroys all active particles
void InfernoEngine::CleanUpParticles()
{
	m_pGraphicsManager->CleanUpParticles();
}

void InfernoEngine::SetWindowStyle(UINT style)
{
	m_pGraphicsManager->SetWindowStyle((WINDOW_STYLE)style);
}
void InfernoEngine::SetWindowDimensions(float width, float height)
{
	m_pGraphicsManager->SetWindowDimensions(width, height);
}
void InfernoEngine::SetVSync(bool vsync)
{
	m_pGraphicsManager->SetVSync(vsync);
}
void InfernoEngine::SetAnisotropicFiltering(unsigned int anisotropic)
{
	m_pGraphicsManager->SetAnisotropicFiltering(anisotropic);
}
void InfernoEngine::SetGamma(float gamma)
{
	m_pGraphicsManager->SetGamma(gamma);
}

Vector2 InfernoEngine::GetWindowDimensions() const
{
	return Vector2(m_pGraphicsManager->GetWindowWidth(), m_pGraphicsManager->GetWindowHeight());
}
UINT InfernoEngine::GetWindowStyle() const
{
	return m_pGraphicsManager->GetWindowStyle();
}
bool InfernoEngine::IsWindowDirty() const
{
	return m_pGraphicsManager->IsWindowDirty();
}
UINT InfernoEngine::GetNumDisplays() const
{
	return m_pGraphicsManager->GetNumDisplays();
}
void InfernoEngine::GetSupportedResolutions(std::vector<std::wstring>& resolutions, UINT displayID) const
{
	m_pGraphicsManager->GetSupportedResolutions(resolutions, displayID);
}
bool InfernoEngine::VSyncEnabled() const
{
	return m_pGraphicsManager->VSyncEnabled();
}
unsigned int InfernoEngine::GetAnisotropicFiltering() const
{
	return m_pGraphicsManager->GetAnisotropicFiltering();
}
float InfernoEngine::GetGamma() const
{
	return m_pGraphicsManager->GetGamma();
}

//  IN : Ray - The ray to check collision against
//  OUT : RayCastHit - information about what the ray hit
//
//	returns true if the ray hit a collider, false if it didn't
bool InfernoEngine::RayCast(Ray& ray, RayCastHit& hitInfo){
	return m_pPhysics->Raycast(ray, hitInfo);
}

// IN: origin - the origin of the sphere
//	   radius - the radius of the sphere
// OUT: hitinfo - vector of information about what the sphere overlapped with
// 
// Checks to see if a ray collides with an object
bool InfernoEngine::SphereOverlap(const Vector3& origin, const float radius, std::vector<RayCastHit>& hitInfo){
	return m_pPhysics->SphereOverlap(origin, radius, hitInfo);
}

// === RegisterForCanvasScaleChange
//  IN : string& - Name of the Canvas to register with
//       void* - The owning object of the function to be called.
//       CanvasScaleChangeFunc - Func to be registered. Format of void(void)
//  OUT : void
//
//  Registers a Function to be called when a specific canvas changes it's scale size. 
void InfernoEngine::RegisterForCanvasScaleChange(std::string& _canvasName, void* _callingObject, std::function<void(void)> _func)
{
	if (m_pUIManager == nullptr) {
		return;
	}

	m_pUIManager->RegisterForCanvasScaleChange(_canvasName, _callingObject, _func);
}

// === UnregisterFromCanvasScaleChange
//  IN : string& - Name of the Canvas to unregister from
//       void* - The owning object of the function that was registered.
//  OUT : void
// 
//  Unregisters a CanvasScaleChange Function from a specific canvas.
void InfernoEngine::UnregisterFromCanvasScaleChange(std::string& _canvasName, void* _callingObject)
{
	if (m_pUIManager == nullptr) {
		return;
	}

	m_pUIManager->UnregisterFromCanvasScaleChange(_canvasName, _callingObject);
}

void InfernoEngine::ClearView()
{
	if (m_pUIManager == nullptr) {
		return;
	}

	m_pUIManager->ClearView();
}

bool InfernoEngine::IsFocusedElementLocked()
{
	if (m_pUIManager == nullptr) {
		return false;
	}

	return m_pUIManager->IsFocusedElementLocked();
}

// IN: HWND   - Handle to the window the message is for
//     UINT   - Message for the window
//     WPARAM - Message data
//     LPARAM - Message data
// OUT: LRESULT - Result of the message processing
//     
// Our WndProc function for handling messages for the window
LRESULT InfernoEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		m_pGraphicsManager->SetWindowDimensions(m_pGraphicsManager->m_fWidth, m_pGraphicsManager->m_fHeight);
		break;
	case WM_CLOSE:
		Inferno::ExitApp();
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		// Notifies the input manager that the current window has been destroyed and to set it's pointer to null
		InputManager::SetWindow(nullptr);
		PostQuitMessage(0);
		break;
	case WM_MOVE:
		InputManager::UpdateWindowPosition(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KILLFOCUS:
		InputManager::LostWindowFocus(true);
		break;
	case WM_SETFOCUS:
		InputManager::LostWindowFocus(false);
		m_pGraphicsManager->SetFullscreen(m_pGraphicsManager->m_pWindow->GetWindowStyle() == WINDOW_STYLE_FULLSCREEN ? true : false);
		break;
	case WM_SYSKEYDOWN:
		// check to see if we are trying to peform an Alt + Enter to transition to full screen
		// The 30th bit in lParam lets us know if Alt is currently being held down when the current key is pressed
		// so we don't need to do any double checks here as Windows has done that for us.
		if(wParam == VK_RETURN && ((lParam & (1 << 29)) != 0))
		{
			bool fullscreen = !(m_pGraphicsManager->m_pWindow->GetWindowStyle() == WINDOW_STYLE_FULLSCREEN);
			m_pGraphicsManager->SetWindowStyle((fullscreen ? WINDOW_STYLE_FULLSCREEN : WINDOW_STYLE_WINDOWED));
			m_pGraphicsManager->ApplyWindowChanges();

			EventSystem::GetInstance()->SendEventNow("FORCED_FULLSCREEN_CHANGE", &fullscreen);
			break;
		}
	case WM_KEYDOWN:
		InputManager::KeyDown(LOWORD(wParam));
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		InputManager::KeyUp(LOWORD(wParam));
		break;
	case WM_RBUTTONDOWN:
		InputManager::MouseButtonDown((int)Button::RightClick);
		break;
	case WM_MBUTTONDOWN:
		InputManager::MouseButtonDown((int)Button::MiddleClick);
		break;
	case WM_LBUTTONDOWN:
		InputManager::MouseButtonDown((int)Button::LeftClick);
		break;
	case WM_XBUTTONDOWN:
		InputManager::MouseButtonDown(HIWORD(wParam) == XBUTTON1 ? (int)Button::MouseButton4 : (int)Button::MouseButton5);
		break;
	case WM_LBUTTONUP:
		InputManager::MouseButtonUp((int)Button::LeftClick);
		break;
	case WM_RBUTTONUP:
		InputManager::MouseButtonUp((int)Button::RightClick);
		break;
	case WM_MBUTTONUP:
		InputManager::MouseButtonUp((int)Button::MiddleClick);
		break;
	case WM_XBUTTONUP:
		InputManager::MouseButtonUp(HIWORD(wParam) == XBUTTON1 ? (int)Button::MouseButton4 : (int)Button::MouseButton5);
		break;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			InputManager::MouseWheelDirection(1);
		else
			InputManager::MouseWheelDirection(-1);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void main(){}