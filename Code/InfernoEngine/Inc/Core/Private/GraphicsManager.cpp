#include "GraphicsManager.h"
#include "GlobalIncludes.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ParticleManager.h"
#include "EventSystem.h"
#include "Flags.h"
#include "Win32Window\Win32Window.h"
#include "WICTextureLoader.h"
#include "FileSystem.h"

#include "Assets\Material.h"
#include "Assets\Texture.h"
#include "Assets\Mesh.h"

#include "Font\FontManager.h"
#include "Font\Font.h"

#include "Rendering\PipelineManager.h"
#include "Rendering\RenderingResources.h"

#include "Physics\Physics.h"

#include "Components\Camera.h"
#include "Components\Transform.h"
#include "Components\MeshRenderer.h"
#include "Components\SkinnedMeshRenderer.h"
#include "Components\SpriteRenderer.h"

#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIText.h"
#include "UIElement.h"

#if defined(DEBUG) || defined(_DEBUG)
	#include "FileSystem.h"
	#include "Rendering\DXEvent.h"
	#include <Initguid.h>
	#include <DXGIDebug.h>

	typedef HRESULT(__stdcall *GetDXGIFunction)(const IID&, void**);
#endif

#include <set>

#pragma comment(lib, "dxgi.lib")

#define MAX_MSAA_COUNT 8
#define MIN_MSAA_COUNT 1
#define MAX_MSAA_QUALITY -1
#define MIN_MSAA_QUALITY 0
#define MAX_ANISOTROPIC_COUNT 16
#define MIN_ANISOTROPIC_COUNT 1
#define MIN_GAMMA 0.0f
#define MAX_GAMMA 2.0f

#define MAX_VERTICES 100000
#define MAX_INDICES  300000
#define MAX_SKINNED_VERTS 2000000

#define HIGHEST_SUPPORTED_RES_X 1920
#define HIGHEST_SUPPORTED_RES_Y 1080

#define FRUSTUM_CULLING_DISABLED 0

enum VERTEX_SHADERS { VERTEX_SHADERS_QUAD, VERTEX_SHADERS_PARTICLE, VERTEX_SHADERS_STATIC_MESH, VERTEX_SHADERS_SKINNED_MESH, TOTAL_VERTEX_SHADERS };
enum PIXEL_SHADERS { PIXEL_SHADERS_ULIT, TOTAL_PIXEL_SHADERS };
enum INPUT_LAYOUTS { INPUT_LAYOUTS_QUAD, INPUT_LAYOUTS_PARTICLE, INPUT_LAYOUTS_STATIC_MESH, INPUT_LAYOUTS_SKINNED_MESH, TOTAL_INPUT_LAYOUTS };
enum BUFFERS { BUFFERS_VERTEX, BUFFERS_INDEX, BUFFERS_WORLD, BUFFERS_VIEW, BUFFERS_PROJECTION, BUFFERS_BONES_MATRICES, BUFFERS_COLOR, TOTAL_BUFFERS };

// Default CTor
GraphicsManager::GraphicsManager() : m_rRendererRegistery(50), m_rSkinnedRendererRegistry(50), m_rUIRegistry(50), m_rAlphaRendererRegistery(50)
{
	// Set up default values
	m_eGraphicsStatus = MANAGER_STATUS_UNINITALIZED;
	m_pWindow = nullptr;
	m_ePrevWindowStyle = WINDOW_STYLE_WINDOWED;
	m_fPrevWidth = m_fWidth = 800;
	m_fPrevHeight = m_fHeight = 600;
	m_fAspectRatio = m_fWidth / m_fHeight;
	m_uiMSAACount = 1;
	m_iMSAAQuality = 0;
	m_uiAnisotropicCount = 1;
	m_fGamma = 1.0f;

	m_pContext = nullptr;
	m_pDevice = nullptr;
	m_pSwapChain = nullptr;
	m_pVideoOutput = nullptr;

	m_pResources = nullptr;
	m_pPipelineManager = nullptr;
	m_pLightManager = nullptr;
	m_pParticleManager = nullptr;
	m_pRenderTree = nullptr;

	m_lHead = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	m_pAnnotation = nullptr;	// Pointer to the custom annotations for DirectX calls
	m_pDebug = nullptr;
#endif

	m_uiPrevIndexCount = m_uiPrevVertCount = 0;

	m_vVertexEntryList.reserve(100);
	m_vVertexEntryList.push_back({ 0, MAX_SKINNED_VERTS });

	m_pDevice = nullptr;
	m_pContext = nullptr;
	m_pSwapChain = nullptr;

	// Creates a new instance of the flags class
	m_fcGraphicsFlags = new Flags<char>(TOTAL_FLAGS);
}

// DTor
GraphicsManager::~GraphicsManager()
{
	Destroy();

#if defined(DEBUG) || defined(_DEBUG)
	//After everything has been released, check for live objects
	if (m_pDebug)
		m_pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	else
		PRINT("Unable to report live objects as the DXGI Debug object wasn't created!\n");

	SAFE_RELASE(m_pDebug);
#endif

	delete m_fcGraphicsFlags;
}

// IN: void
// OUT: bool - States if we were able to create our device or not
//
// Creates our device and context
bool GraphicsManager::InitializeDevice()
{
	HRESULT hr = S_OK;

	const D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIFactory1* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;

	if (!FindAdapter(&factory, &adapter))
		return false;

	if (!SUCCEEDED(hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, lvl, ARRAYSIZE(lvl), D3D11_SDK_VERSION, &m_pDevice, &FeatureLevelsSupported, &m_pContext)))
	{
		ALERT("Failed to create the D3D11 Device!");
		PRINT("Failed to create the D3D11 Device!\n");

		return false;
	}

	adapter->EnumOutputs(0, &m_pVideoOutput);

	SET_DEBUG_NAME(m_pDevice, "Device")
	SET_DEBUG_NAME(m_pContext, "Context")

	SAFE_RELASE(factory);
	SAFE_RELASE(adapter);

	return true;
}

//// IN: void
//// OUT: bool - States window creation success
////
//// Creates a window for the game to use
//bool GraphicsManager::CreateGameWindow()
//{
//	if (m_hWindow)
//		return false;
//
//	PRINT("Creating Window...");
//
//	// Create our window class
//	/*WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = m_pWndProc;
//	wcex.cbClsExtra = 0;
//	wcex.cbWndExtra = 0;
//	wcex.hInstance = m_hInst;
//	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wcex.lpszMenuName = nullptr;
//
//	if (m_pClassName != nullptr || m_pClassName != L"")
//		wcex.lpszClassName = m_pClassName;
//	else
//	{
//		wcex.lpszClassName = L"Inferno Engine";
//
//		delete[] m_pClassName;
//		m_pClassName = _wcsdup(wcex.lpszClassName);
//	}
//
//	if (m_pIconName != nullptr || m_pIconName != L"")
//		wcex.hIcon = LoadIcon(m_hInst, m_pIconName);
//
//	if (m_pSmallIconName != nullptr || m_pSmallIconName != L"")
//		wcex.hIconSm = LoadIcon(m_hInst, m_pSmallIconName);
//
//	if (ASSERT(!RegisterClassEx(&wcex), L"Failed to register the Windows Class!"))
//	{
//		// If the window class fails to register
//		return false;
//	}
//
//	int width = (int)m_fWidth;
//	int height = (int)m_fHeight;
//	int windowFlags = 0;
//
//	// Grabs the desktop dimentions
//	RECT desktop;
//	GetWindowRect(GetDesktopWindow(), &desktop);
//
//	// Set up a default window Rect
//	RECT window_size = { 0, 0, width, height };
//
//	// Set up the flags for the window based on our style of window
//	switch (m_eWindowStyle)
//	{
//	case WINDOW_STYLE_FULLSCREEN:
//	case WINDOW_STYLE_WINDOWED:
//	{
//		windowFlags = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
//
//		// Adjust our windows rect based on the set flags
//		AdjustWindowRect(&window_size, windowFlags, false);
//
//		int windowWidth = window_size.right - window_size.left;
//		int windowHeight = window_size.bottom - window_size.top;
//		int left = (desktop.right - windowWidth) >> 1;
//		int top = (desktop.bottom - windowHeight) >> 1;
//
//		m_hWindow = CreateWindow(m_pClassName, m_pClassName, windowFlags,
//			left, top, windowWidth, windowHeight, NULL, NULL, m_hInst, this);
//	}
//		break;
//		// If we are borderless, get the desktop width and height to use as the windows width and height
//	case WINDOW_STYLE_BORDERLESS:
//	{
//		m_fWidth = (float)desktop.right;
//		m_fHeight = (float)desktop.bottom;
//		m_fAspectRatio = m_fHeight / m_fWidth;
//		windowFlags = WS_POPUP;
//
//		m_hWindow = CreateWindow(m_pClassName, m_pClassName, windowFlags,
//			0, 0, (int)m_fWidth, (int)m_fHeight, NULL, NULL, m_hInst, this);
//	}
//	break;
//	}
//
//	if (ASSERT(!m_hWindow, L"Failed to create the Window!"))
//	{
//		// If the window fails to create
//		return false;
//	}
//
//	ShowWindow(m_hWindow, SW_SHOW);
//	UpdateWindow(m_hWindow);*/
//
//	PRINT("Success!\n");
//
//	return true;
//}

// IN: void
// OUT: bool - States DirectX setup success
//
// Sets up DirectX
bool GraphicsManager::InitializeDirectX()
{
	PRINT("Initializing DirectX...");

	// Create a new resource bank
	m_pResources = new RenderingResources();

	// Creates our device, context, and swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = (UINT)m_fWidth;
	swapChainDesc.BufferDesc.Height = (UINT)m_fHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = (UINT)m_uiMSAACount;
	swapChainDesc.SampleDesc.Quality = (UINT)m_iMSAAQuality;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = m_pWindow->GetHWnd();
	swapChainDesc.Windowed = m_eGraphicsStatus == WINDOW_STYLE_FULLSCREEN ? FALSE : TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = 0;

	/*hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, lvl, _countof(lvl), D3D11_SDK_VERSION, &swapChainDesc,  &m_pSwapChain.p, &m_pDevice.p, &FeatureLevelsSupported, &m_pContext.p);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Device and SwapChain!!"))
		return false;*/

	IDXGIDevice* pDXGIDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	if (!SUCCEEDED(hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice*), reinterpret_cast<void**>(&pDXGIDevice))))
	{
		ASSERT(false, "Failed to get the DXGI Device from the D3D11 Device!");

		return false;
	}

	if (!SUCCEEDED(hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pAdapter))))
	{
		ASSERT(false, "Failed to get the DXGI Adapter from the DXGI Device!");

		return false;
	}

	if (!SUCCEEDED(hr = pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pFactory))))
	{
		ASSERT(false, "Failed to get the DXGI Adapter from the DXGI Device!");

		return false;
	}

	InitOutputModes(swapChainDesc.BufferDesc);
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (!ASSERT(SUCCEEDED(pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain)), "Failed to create the Swap Chain!"))
	{
		SAFE_RELASE(pDXGIDevice);
		SAFE_RELASE(pAdapter);
		SAFE_RELASE(pFactory);

		return false;
	}

	hr = pFactory->MakeWindowAssociation(swapChainDesc.OutputWindow, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	SET_DEBUG_NAME(m_pSwapChain, "SwapChain")

	SAFE_RELASE(pDXGIDevice);
	SAFE_RELASE(pAdapter);
	SAFE_RELASE(pFactory);

#if defined(DEBUG) || defined(_DEBUG)
	static_cast<ID3D11DeviceContext *>(m_pContext)->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void **)&m_pAnnotation);

	//Done this way so the current device does not get an extra reference 
	HMODULE m_hModule = LoadLibraryEx(L"Dxgidebug.dll", 0, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE);
	if (nullptr == m_hModule) {
		PRINT("Could not find Dxgidebug.dll\n");
		return false;
	}

	GetDXGIFunction GetDXGI = reinterpret_cast<GetDXGIFunction>(GetProcAddress(m_hModule, "DXGIGetDebugInterface"));
	if (GetDXGI)
		GetDXGI(__uuidof(IDXGIDebug), (void**)&m_pDebug);
	else
		PRINT("Warning: Live objects not collected, D3D11_CREATE_DEVICE_DEBUG flag not set\n");

	FreeLibrary(m_hModule);
#endif

	// Create a new pipeline manager sending it the context to use to apply changes to the pipeline
	m_pPipelineManager = new PipelineManager(m_pContext);

	// Pass along the created device and swapchain to our resource bank so it can use them to create and
	// apply changes as needed
	m_pResources->SetDevice(m_pDevice);
	m_pResources->SetContext(m_pContext);

	// Sets up our back buffer
	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	m_iRenderTarget = m_pResources->CreateRenderTargetView(L"MainRTV", pBackBuffer, nullptr);
	m_pPipelineManager->SetRenderTargetViews(1, &m_iRenderTarget);
	SAFE_RELEASE(pBackBuffer);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Backbuffer!!"))
		return false;

	// Sets up our default viewport
	D3D11_VIEWPORT screenViewport;
	screenViewport.Width = m_fWidth;
	screenViewport.Height = m_fHeight;
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	int ViewID = m_pResources->AddViewport(screenViewport);
	m_pPipelineManager->SetViewport(1, &ViewID);

	// Set up our Depth Stencil View
	ID3D11Texture2D* depthBuffer;
	D3D11_TEXTURE2D_DESC dBuff;
	ZeroMemory(&dBuff, sizeof(D3D11_TEXTURE2D_DESC));
	dBuff.Width = (UINT)m_fWidth;
	dBuff.Height = (UINT)m_fHeight;
	dBuff.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dBuff.ArraySize = 1;
	dBuff.MipLevels = 1;
	dBuff.Usage = D3D11_USAGE_DEFAULT;
	dBuff.SampleDesc.Count = 1;//(UINT)m_uiMSAACount;
	dBuff.SampleDesc.Quality = 0;//(UINT)m_iMSAAQuality;
	dBuff.Format = DXGI_FORMAT_R32_TYPELESS;

	hr = m_pDevice->CreateTexture2D(&dBuff, NULL, &depthBuffer);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Backbuffer Depth Stencil View!!"))
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_iDepthStencilView = m_pResources->CreateDepthStencilView(L"MainDSV", depthBuffer, &descDSV);
	m_pPipelineManager->SetDepthStencilView(m_iDepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;//DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* DepthSRV = nullptr;
	m_pDevice->CreateShaderResourceView(depthBuffer, &srvDesc, &DepthSRV);
	m_pResources->AddImage(L"DepthMap", DepthSRV);
	SAFE_RELEASE(depthBuffer);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Backbuffer Depth Stencil View!!"))
		return false;

	// Set up our Sampler State
	D3D11_SAMPLER_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(D3D11_SAMPLER_DESC));
	sDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.MinLOD = (-FLT_MAX);
	sDesc.MaxLOD = (FLT_MAX);
	sDesc.MaxAnisotropy = (UINT)m_uiAnisotropicCount;
	sDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	int SamplerID = m_pResources->CreateSamplerState(L"WrapSamplerState", &sDesc);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Sample State!!"))
		return false;

	D3D11_RASTERIZER_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(D3D11_RASTERIZER_DESC));
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_BACK;
	rDesc.FrontCounterClockwise = FALSE;
	rDesc.DepthClipEnable = TRUE;
	rDesc.ScissorEnable = FALSE;
	rDesc.MultisampleEnable = m_fcGraphicsFlags->GetFlag(MSAA_ENBALED);
	rDesc.AntialiasedLineEnable = m_fcGraphicsFlags->GetFlag(ANTI_ALIASING_ENABLED);

	int RasterizerID = m_pResources->CreateRasterizerState(L"FillRasterizerState", &rDesc);

	rDesc.FillMode = D3D11_FILL_WIREFRAME;
	rDesc.CullMode = D3D11_CULL_NONE;

	m_pResources->CreateRasterizerState(L"WireFrameRasterizerState", &rDesc);

	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_FRONT;

	m_pResources->CreateRasterizerState(L"SkyboxRasterizerState", &rDesc);

	rDesc.CullMode = D3D11_CULL_NONE;
	
	int AlphaMeshRasterizerID = m_pResources->CreateRasterizerState(L"AlphaMeshRasterizerState", &rDesc);

	D3D11_BLEND_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BLEND_DESC));
	bDesc.AlphaToCoverageEnable = false;
	bDesc.IndependentBlendEnable = false;
	bDesc.RenderTarget[0].BlendEnable = true;
	bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	int BlendID = m_pResources->CreateBlendState(L"AlphaBlendingBlendState", &bDesc);

	bDesc.AlphaToCoverageEnable = false;
	bDesc.IndependentBlendEnable = false;
	bDesc.RenderTarget[0].BlendEnable = false;
	bDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	int NoBlendID = m_pResources->CreateBlendState(L"NoBlendingBlendState", &bDesc);

	bDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	m_pResources->CreateBlendState(L"DepthOnlyAlphaTestState", &bDesc);

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;//D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_pResources->CreateDepthStencilState(L"ObjectStencilState", &dsDesc);

	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER;

	m_pResources->CreateDepthStencilState(L"SkyboxStencilState", &dsDesc);

	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;

	m_iDepthPrepassDepthStencilState = m_pResources->CreateDepthStencilState(L"DepthLessOnlyStencilState", &dsDesc);

	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	m_iAlphaDepthStencilState = m_pResources->CreateDepthStencilState(L"DepthLessEqualStencilState", &dsDesc);

	// Load our shaders
	int QuadID = m_pResources->LoadShader(VERTEX_SHADER, L"QuadVertexShader");
	int PartID = m_pResources->LoadShader(VERTEX_SHADER, L"ParticleVertexShader");
	int StatID = m_pResources->LoadShader(VERTEX_SHADER, L"StaticMeshVertexShader");
	int SkinID = m_pResources->LoadShader(VERTEX_SHADER, L"SkinnedMeshVertexShader");
	UIVertexID = m_pResources->LoadShader(VERTEX_SHADER, L"UIVertexShader");
	m_iPositionOnlyVS = m_pResources->LoadShader(VERTEX_SHADER, L"LineVertexShader");
	m_iAlphaTestVertexShader = m_pResources->LoadShader(VERTEX_SHADER, L"AlphaPrepassVertexShader");
	m_pResources->LoadShader(VERTEX_SHADER, L"SkyboxVertexShader");

	UIGeometryID = m_pResources->LoadShader(GEOMETRY_SHADER, L"UIGeometryShader");
	m_pResources->LoadShader(GEOMETRY_SHADER, L"ParticleGeometryShader");
	int StreamOutputGShader = m_pResources->LoadShader(GEOMETRY_SHADER, L"StreamOutputSkinnedGeometryShader");
	m_pResources->LoadShader(GEOMETRY_SHADER, L"QuadGeometryShader");
	m_pResources->LoadShader(GEOMETRY_SHADER, L"QuadBillboardedGeometryShader");
	m_pResources->LoadShader(GEOMETRY_SHADER, L"ScreenQuadGeometryShader");

	int UnlitID = m_pResources->LoadShader(PIXEL_SHADER, L"UnlitPixelShader");
	m_iAlphaTestPixelShader = m_pResources->LoadShader(PIXEL_SHADER, L"AlphaTestPixelShader");
	int UIPixID = m_pResources->LoadShader(PIXEL_SHADER, L"UIPixelShader");
	int UIFillPixID = m_pResources->LoadShader(PIXEL_SHADER, L"UIFillPixelShader");
	int UIFillRightPixID = m_pResources->LoadShader(PIXEL_SHADER, L"UIFillRightPixelShader");
	int UIFillTopPixID = m_pResources->LoadShader(PIXEL_SHADER, L"UIFillTopPixelShader");
	m_pResources->LoadShader(PIXEL_SHADER, L"ColorOnlyPixelShader");
	m_pResources->LoadShader(PIXEL_SHADER, L"SkyboxPixelShader");
	int litID = m_pResources->LoadShader(PIXEL_SHADER, L"LightingPixelShader");
	m_pResources->LoadShader(PIXEL_SHADER, L"ParticlePixelShader");
	int UIDualImageFillPixID = m_pResources->LoadShader(PIXEL_SHADER, L"UIDualImageFillPixelShader");
	m_pResources->LoadShader(PIXEL_SHADER, L"TiledParticlePixelShader");
	int UIOcclusionID = m_pResources->LoadShader(PIXEL_SHADER, L"UIOcclusionPixelShader");
	int UIRadialID = m_pResources->LoadShader(PIXEL_SHADER, L"UIRadialFillPixelShader");
	int UIRadialCID = m_pResources->LoadShader(PIXEL_SHADER, L"UIRadialFillClockwisePixelShader");

	m_pResources->LoadShader(COMPUTE_SHADER, L"LightComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"InitDeadParticlesComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"ParticleSimulationComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"CoarseCullingComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"ParticleCullingComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"ParticleTileRenderingComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"SphereParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"SphereEdgeParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"HemisphereParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"HemisphereEdgeParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"CircleParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"CircleEdgeParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"ConeParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"ConeOutterParticleEmitterComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"OutlineFontComputeShader");
	m_pResources->LoadShader(COMPUTE_SHADER, L"DestroyAllParticlesComputeShader");

	// Now that all shaders are loaded, have the reflection set up our constant buffers
	// We can request the buffers by name to get the IDs
	m_pResources->ApplyConstantBuffers();

	D3D11_SO_DECLARATION_ENTRY entry[] = {
		{ 0, "SV_POSITION", 0, 0, 4, 0 },
		{ 0, "NORMAL", 0, 0, 3, 0 },
		{ 0, "TANGENT", 0, 0, 3, 0 },
		{ 0, "TEXTCOORD", 0, 0, 2, 0 },
		{ 0, "BONEINFLUENCES", 0, 0, 4, 0 },
		{ 0, "BONEWEIGHTS", 0, 0, 4, 0 },
	};

	UINT strides[1] = { sizeof(Vertex) };
	m_pResources->CreateGeometryShaderWithStreamOutput(StreamOutputGShader, entry, 6, strides, 1);

	WorldBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"World"))->GetBuffer();
	ViewBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"View"))->GetBuffer();
	//BillboardBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"BillboardRotation"))->GetBuffer();
	ProjectionBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"Projection"))->GetBuffer();
	BonesBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"Bones"))->GetBuffer();
	ColorBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"Color"))->GetBuffer();
	UVOffsetBuffer = m_pResources->GetConstantBufferByID(m_pResources->GetConstantBufferByName(L"UVOffsets"))->GetBuffer();

	D3D11_INPUT_ELEMENT_DESC quadLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC alphaLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC lineLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC staticLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC skinnedLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINFLUENCES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UIInputID = m_pResources->CreateInputLayout(L"UILayout", quadLayout, 0, UIVertexID);
	m_pResources->CreateInputLayout(L"QuadLayout", quadLayout, 3, QuadID);
	m_iAlphaInputLayout = m_pResources->CreateInputLayout(L"AlphaPrepassLayout", alphaLayout, 2, m_iAlphaTestVertexShader);
	m_iPositionOnlyInputLayout = m_pResources->CreateInputLayout(L"LineLayout", lineLayout, 1, m_iPositionOnlyVS);
	int inputID = m_pResources->CreateInputLayout(L"StaticMeshLayout", staticLayout, 4, StatID);
	m_pResources->CreateInputLayout(L"SkinnedMeshLayout", skinnedLayout, 6, SkinID);

	// Set up our Vertex and Index Buffers
	D3D11_BUFFER_DESC vBuffer;
	ZeroMemory(&vBuffer, sizeof(D3D11_BUFFER_DESC));
	vBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBuffer.ByteWidth = sizeof(Vertex) * MAX_VERTICES;
	vBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBuffer.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_BUFFER_DESC iBuffer;
	ZeroMemory(&iBuffer, sizeof(D3D11_BUFFER_DESC));
	iBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBuffer.ByteWidth = sizeof(unsigned int) * MAX_INDICES;
	iBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	iBuffer.Usage = D3D11_USAGE_DYNAMIC;

	int VertexID = m_pResources->CreateVertexBuffer(L"VertexBuffer", &vBuffer);
	int IndexID = m_pResources->CreateIndexBuffer(L"IndexBuffer", &iBuffer);

	VertexBuffer = m_pResources->GetBufferByID(VERTEX_BUFFER, VertexID)->GetBuffer();
	IndexBuffer = m_pResources->GetBufferByID(INDEX_BUFFER, IndexID)->GetBuffer();

	struct staticVert
	{
		Vector4 Pos;
		Vector3 Norm;
		Vector3 Tan;
		Vector2 Uv;
	};
	vBuffer.ByteWidth = (sizeof(Vertex)) * MAX_SKINNED_VERTS;
	vBuffer.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	vBuffer.CPUAccessFlags = 0;
	vBuffer.Usage = D3D11_USAGE_DEFAULT;
	SkinnedVertexBufferID = m_pResources->CreateVertexBuffer(L"SkinnedVertexBuffer", &vBuffer);

	SkinnedVertexBuffer = m_pResources->GetBufferByID(SkinnedVertexBufferID)->GetBuffer();

	// Create the light buffer for the Position and Radius of the light
	D3D11_BUFFER_DESC structBuff;
	ZeroMemory(&structBuff, sizeof(D3D11_BUFFER_DESC));
	structBuff.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	structBuff.ByteWidth = sizeof(Vector4) * MAX_LIGHTS;
	structBuff.StructureByteStride = sizeof(Vector4);
	structBuff.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	structBuff.Usage = D3D11_USAGE_DYNAMIC;
	structBuff.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	int sBuffID = m_pResources->CreateStructuredBuffer(L"LightCentersAndRadii", &structBuff);
	int lightSRV = m_pResources->CreateBufferSRV(sBuffID);

	sBuffID = m_pResources->CreateStructuredBuffer(L"LightColorAndIntensity", &structBuff);
	lightSRV = m_pResources->CreateBufferSRV(sBuffID);

	ZeroMemory(&structBuff, sizeof(D3D11_BUFFER_DESC));
	structBuff.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	structBuff.ByteWidth = sizeof(unsigned int) * MAX_LIGHTS_PER_CELL * NUM_CELLS;
	structBuff.StructureByteStride = sizeof(unsigned int);
	structBuff.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	sBuffID = m_pResources->CreateStructuredBuffer(L"LightIndiceLists", &structBuff);
	int lightUAV = m_pResources->CreateUnorderedAccessView(sBuffID);
	lightSRV = m_pResources->CreateBufferSRV(sBuffID);

	ZeroMemory(&structBuff, sizeof(D3D11_BUFFER_DESC));
	structBuff.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	structBuff.ByteWidth = sizeof(ParticlePartA) * MAX_PARTICLES;
	structBuff.StructureByteStride = sizeof(ParticlePartA);
	structBuff.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticlePartABuffer", &structBuff);
	m_pResources->CreateUnorderedAccessView(sBuffID);
	m_pResources->CreateBufferSRV(sBuffID);

	structBuff.ByteWidth = sizeof(ParticlePartB) * MAX_PARTICLES;
	structBuff.StructureByteStride = sizeof(ParticlePartB);

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticlePartBBuffer", &structBuff);
	m_pResources->CreateUnorderedAccessView(sBuffID);

	ZeroMemory(&structBuff, sizeof(D3D11_BUFFER_DESC));
	structBuff.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	structBuff.ByteWidth = sizeof(UINT) * MAX_PARTICLES;
	structBuff.StructureByteStride = sizeof(UINT);
	structBuff.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticleDeadList", &structBuff);
	m_pResources->CreateUnorderedAccessView(sBuffID, D3D11_BUFFER_UAV_FLAG_APPEND);

	structBuff.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	structBuff.ByteWidth = sizeof(Vector2) * MAX_PARTICLES;
	structBuff.StructureByteStride = sizeof(Vector2);

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticleActiveIndexList", &structBuff);
	m_pResources->CreateUnorderedAccessView(sBuffID, D3D11_BUFFER_UAV_FLAG_COUNTER);
	m_pResources->CreateBufferSRV(sBuffID);

	ZeroMemory(&structBuff, sizeof(D3D11_BUFFER_DESC));
	structBuff.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	structBuff.ByteWidth = sizeof(Vector4) * MAX_PARTICLES;
	structBuff.StructureByteStride = sizeof(Vector4);
	structBuff.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticleViewSpacePositions", &structBuff);
	m_pResources->CreateUnorderedAccessView(sBuffID, D3D11_BUFFER_UAV_FLAG_COUNTER);
	m_pResources->CreateBufferSRV(sBuffID);

	D3D11_BUFFER_DESC buffer;
	ZeroMemory(&buffer, sizeof(D3D11_BUFFER_DESC));
	buffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffer.ByteWidth = sizeof(UINT) * 5;
	buffer.StructureByteStride = sizeof(UINT);
	buffer.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticleDrawArgs", &buffer);
	m_pResources->CreateUnorderedAccessView(sBuffID, 0, DXGI_FORMAT_R32_UINT, &buffer);

	buffer.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	buffer.ByteWidth = sizeof(UINT) * MAX_PARTICLES * MAX_CULLING_TILES;

	sBuffID = m_pResources->CreateStructuredBuffer(L"CoarseTiledIndexBuffer", &buffer);
	m_pResources->CreateUnorderedAccessView(sBuffID, 0, DXGI_FORMAT_R32_UINT, &buffer);
	m_pResources->CreateBufferSRV(sBuffID, DXGI_FORMAT_R32_UINT, &buffer);

	buffer.ByteWidth = sizeof(UINT) * MAX_CULLING_TILES;

	sBuffID = m_pResources->CreateStructuredBuffer(L"CoarseTiledBufferCounters", &buffer);
	m_pResources->CreateUnorderedAccessView(sBuffID, 0, DXGI_FORMAT_R32_UINT, &buffer);
	m_pResources->CreateBufferSRV(sBuffID, DXGI_FORMAT_R32_UINT, &buffer);

	UINT maxCullingTiles = align(HIGHEST_SUPPORTED_RES_X, CULLING_TILE_SIZE_X) / CULLING_TILE_SIZE_X * align(HIGHEST_SUPPORTED_RES_Y, CULLING_TILE_SIZE_Y) / CULLING_TILE_SIZE_Y;
	buffer.ByteWidth = sizeof(UINT) * maxCullingTiles * PARTICLES_PER_TILE_BUFFER;

	sBuffID = m_pResources->CreateStructuredBuffer(L"TiledIndexBuffer", &buffer);
	m_pResources->CreateUnorderedAccessView(sBuffID, 0, DXGI_FORMAT_R32_UINT, &buffer);
	m_pResources->CreateBufferSRV(sBuffID, DXGI_FORMAT_R32_UINT, &buffer);

	buffer.ByteWidth = sizeof(Vector4) * HIGHEST_SUPPORTED_RES_X * HIGHEST_SUPPORTED_RES_Y;
	buffer.StructureByteStride = sizeof(Vector4);

	sBuffID = m_pResources->CreateStructuredBuffer(L"ParticleTextureBuffer", &buffer);
	m_pResources->CreateUnorderedAccessView(sBuffID, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, &buffer);
	m_pResources->CreateBufferSRV(sBuffID, DXGI_FORMAT_R32G32B32A32_FLOAT, &buffer);

	// Create a random values texture for use by compute shaders
	{
		ID3D11Texture2D* randValTexture = nullptr;
		ID3D11ShaderResourceView* randValSRV = nullptr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = 1024;
		desc.Height = 1024;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// Create 4 random values per pixel in the texture so it can be sampled later by shaders looking
		// for pseudo random values
		float *values = new float[desc.Width * desc.Height * 4];
		float *ptr = values;
		for (UINT i = 0; i < desc.Width * desc.Height; i++, ptr += 4)
		{
			ptr[0] = RandFloat(-1.0f, 1.0f);
			ptr[1] = RandFloat(-1.0f, 1.0f);
			ptr[2] = RandFloat(-1.0f, 1.0f);
			ptr[3] = RandFloat(-1.0f, 1.0f);
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = values;
		data.SysMemPitch = desc.Width * 16;
		data.SysMemSlicePitch = 0;

		m_pDevice->CreateTexture2D(&desc, &data, &randValTexture);

		delete[] values;

		if (randValTexture)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srv;
			srv.Format = desc.Format;
			srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srv.Texture2D.MipLevels = 1;
			srv.Texture2D.MostDetailedMip = 0;

			m_pDevice->CreateShaderResourceView(randValTexture, &srv, &randValSRV);

			if (randValSRV)
				m_pResources->AddImage(L"RandomValueTexture", randValSRV);
		}

		SAFE_RELASE(randValTexture);
	}

	m_pPipelineManager->CreateEffect("StaticMesh_Unlit", UnlitID, RasterizerID, NoBlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("StaticMesh_Diffuse", litID, RasterizerID, NoBlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("StaticMesh_DiffuseBlend", litID, AlphaMeshRasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("SkinnedMesh_Unlit", UnlitID, RasterizerID, NoBlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("SkinnedMesh_Diffuse", litID, RasterizerID, NoBlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("Particle_Unlit", UnlitID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_Unlit", UnlitID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_Fill_Unlit", UIFillPixID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_FillRight_Unlit", UIFillRightPixID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_FillTop_Unlit", UIFillTopPixID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_FillDualImage_Unlit", UIDualImageFillPixID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_Occlusion_Unlit", UIOcclusionID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_RadialFill_Unlit", UIRadialID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("UI_RadialFillClockwise_Unlit", UIRadialCID, RasterizerID, BlendID, &SamplerID, 1);
	m_pPipelineManager->CreateEffect("StaticMesh_UnlitBlend", UnlitID, AlphaMeshRasterizerID, BlendID, &SamplerID, 1);

	m_pPipelineManager->CreateEffect("StreamOutEffect", -1, -1, -1, nullptr, 0);

	// Since everything set up correctly, we now can register for the events we need to know about
	EventSystem::GetInstance()->RegisterForEvent("MAIN_CAMERA_VIEW_UPDATE", this, std::bind(&GraphicsManager::UpdateViewMatrix, this, std::placeholders::_1));
	EventSystem::GetInstance()->RegisterForEvent("MAIN_CAMERA_PROJECTION_UPDATE", this, std::bind(&GraphicsManager::UpdateProjectionMatrix, this, std::placeholders::_1));

	PRINT("Success!\n");

	return true;
}

// IN: IDXGIFactory1** - Pointer to the Factory Pointer
//     IDXGIAdapter1** - Pointer to the Adapter Pointer
// OUT: bool - Success on finding the adapter
//
// Finds the bets adapter to use
bool GraphicsManager::FindAdapter(IDXGIFactory1** factory, IDXGIAdapter1** adapter)
{
	HRESULT hr = S_OK;
	IDXGIAdapter1* temp = nullptr;
	DXGI_ADAPTER_DESC1 bestDesc;

	if (!ASSERT(SUCCEEDED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory))), "Failed to create the IDXGIFactory!"))
		return false;

	UINT index = 0;
	while ((*factory)->EnumAdapters1(index++, &temp) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC1));
		temp->GetDesc1(&desc);

		if (*adapter == nullptr)
		{
			*adapter = temp;
			bestDesc = desc;
		}
		else if (bestDesc.DedicatedSystemMemory < desc.DedicatedSystemMemory)
		{
			(*adapter)->Release();
			*adapter = temp;
			bestDesc = desc;
		}
		else
			temp->Release();
	}

	return true;
}

// IN: DXGI_MODE_DESC& - Output Mode to check against
// OUT: void
//
// Gets all supported output modes and applies closest match
void GraphicsManager::InitOutputModes(DXGI_MODE_DESC& mode)
{
	UINT num = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	m_pVideoOutput->GetDisplayModeList(format, flags, &num, 0);

	if (num > 0)
	{
		m_vDisplayModes.resize(num);
		m_pVideoOutput->GetDisplayModeList(format, flags, &num, &m_vDisplayModes[0]);

		UINT i = 0;
		for (; i < num; i++)
		{
			if (mode.Format == m_vDisplayModes[i].Format &&
				mode.Width == m_vDisplayModes[i].Width &&
				mode.Height == m_vDisplayModes[i].Height)
			{
				break;
			}
		}

		if (i == num)
		{
			DXGI_MODE_DESC start = mode;
			m_pVideoOutput->FindClosestMatchingMode(&start, &mode, m_pDevice);
		}
	}
}

// IN: DXGI_MODE_DESC& - Output Mode to check against
// OUT: void
//
// Updates the output mode to the closest mode that is supported
void GraphicsManager::GetClosestOutputMode(DXGI_MODE_DESC& mode)
{
	if (m_vDisplayModes[m_vDisplayModes.size() - 1].Width != mode.Width)
		mode.Scaling = DXGI_MODE_SCALING_STRETCHED;
	else
		mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_MODE_DESC prev = mode;
	m_pVideoOutput->FindClosestMatchingMode(&prev, &mode, m_pDevice);
}

// IN: Vertex*      - Array of Vertices to add
//     unsigned int - Number of elements in the array
// OUT: vector<unsigned int> - Array of indices for locations of the added verts
//
// Adds the verts to the vertex buffer and returns the location of each vert in the buffer
unsigned int GraphicsManager::LoadVerts(const Vertex* verts, const size_t numVerts)
{
	if (m_uiPrevVertCount + numVerts > MAX_VERTICES)
		return 0;

	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(VertexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource);
	Vertex* oldVerts = (Vertex*)resource.pData;
	memcpy((void*)&oldVerts[m_uiPrevVertCount], verts, sizeof(Vertex) * numVerts);
	m_pContext->Unmap(VertexBuffer, 0);

	unsigned int startVertex = (unsigned int)m_uiPrevVertCount;
	m_uiPrevVertCount += numVerts;

	return startVertex;
}
// IN: unsigned int* - Array of Indices to add
//     unsigned int  - Number of elements in the array
// OUT: unsigned int - Start location into the index list for the mesh
//
// Adds the indices to the index buffer and returns the start index
unsigned int GraphicsManager::LoadIndices(unsigned int* indexList, size_t numIndicies)
{
	if (indexList == nullptr || numIndicies == 0)
		return 0;

	unsigned int startIndex = (unsigned int)m_uiPrevIndexCount;

	if (m_uiPrevIndexCount + numIndicies > MAX_INDICES)
		return 0;

	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(IndexBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource);
	unsigned int* currIndicies = (unsigned int*)resource.pData;
	memcpy(&currIndicies[m_uiPrevIndexCount], indexList, numIndicies * sizeof(unsigned int));
	m_pContext->Unmap(IndexBuffer, 0);

	m_uiPrevIndexCount += numIndicies;

	return startIndex;
}
// IN: void* - Mesh to queue for loading
//     bool  - If the mesh should be finalized, in some cases we don't want to as with NavMeshes
// OUT: void
//
// Adds a mesh to the queue to be loaded. This should be used by seperate Threads.
void GraphicsManager::QueueLoadMesh(void* mesh, bool finalize)
{
	char* meshData = new char[2 + sizeof(void*)];
	*meshData = 1;						// Store 1 at the beginning to signify a mesh
	*(meshData + 1) = finalize ? 1 : 0;	// Store if we want to finalize the mesh or not
	memcpy(meshData + 2, &mesh, sizeof(void*));		// Store the mesh pointer into the last 4 bytes

	// Create our new Data and add it to the linked list
	LoadedAssetData* newData = new LoadedAssetData();
	newData->Data = meshData;
	newData->Next = nullptr;

	m_mDataMutex.lock();
	LoadedAssetData* curr = m_lHead;

	if (curr == nullptr)
	{
		m_lHead = newData;
	}
	else
	{
		while (curr->Next != nullptr)
			curr = curr->Next;

		curr->Next = newData;
	}

	m_mDataMutex.unlock();
}
// IN: wchar_t* - Name of the texture to load
// OUT: size_t - Index location of the image
//
// Loads the texture from the name that is passed in
int GraphicsManager::LoadTexture(const wchar_t* filename)
{
	return m_pResources->CreateImage(filename);
}
// IN: wchar_t* - Name of hte texture to store in memory
// OUT: void
//
// Loads the texture file into memory to be loaded onto the GPU at a later point. This automatically
// queues the texture file for loading and should be used by Threads.
void GraphicsManager::StoreTextureInMemory(const wchar_t* filename)
{
	FileSystem fs;
	std::wstring filepath = fs.GetTexturesFolderW() + filename;

	if (wcscmp(filename, L"Wall_stone.dds") == 0)
		int debug = 0;

	HANDLE hFile = CreateFileW(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	// Check to see if we failed to open the file
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wstring error = std::wstring(L"Failed to load texture ") + filename + L"!\n";
		PRINT(error.c_str());
		return;
	}

	// Get the file size
	LARGE_INTEGER FileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	FILE_STANDARD_INFO fileInfo;
	if (!GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo))) {
		CloseHandle(hFile);
		return;
	}
	FileSize = fileInfo.EndOfFile;
#else
	GetFileSizeEx(hFile, &FileSize);
#endif

	// Make sure our file is not too big for a 32-bit allocation
	if (FileSize.HighPart > 0)
	{
		std::wstring error = std::wstring(L"Image file ") + filename + L" too large to load!\n";
		PRINT(error.c_str());
		CloseHandle(hFile);
		return;
	}

	// Create a buffer for the data with padding for additional information for later
	char* imageData = new char[FileSize.LowPart + 9];
	
	// Add our additional information into the buffer
	*imageData = 0;									// 0 meaning texture
	*(DWORD*)(imageData + 1) = FileSize.LowPart;	// Store the Size in the next 4 bytes

	if (!ASSERT(imageData != nullptr, "Failed to allocate a buffer for the shader file!"))
		return;

	// Try to read the whole file offsetting the start point by the buffer size
	DWORD BytesRead = 0;
	if (!ReadFile(hFile, imageData + 9, FileSize.LowPart, &BytesRead, nullptr))
	{
		std::wstring error = std::wstring(L"Failed to read the image file ") + filename + L"!\n";
		PRINT(error.c_str());

		CloseHandle(hFile);
		delete[] imageData;
		return;
	}

	// Close the file since we have now read it
	CloseHandle(hFile);

	// Make sure we were able to read the whole file
	if (BytesRead < FileSize.LowPart)
	{
		std::wstring error = std::wstring(L"Failed to read the whole image file ") + filename + L"!\n";
		PRINT(error.c_str());

		delete[] imageData;
		return;
	}

	*(int*)(imageData + 5) = m_pResources->RequestTextureSlot(filename);

	// Create a new asset data and insert into the linked list
	LoadedAssetData* newData = new LoadedAssetData();
	newData->Data = imageData;
	newData->Next = nullptr;

	m_mDataMutex.lock();
	LoadedAssetData* curr = m_lHead;

	if (curr == nullptr)
	{
		m_lHead = newData;
	}
	else
	{
		while (curr->Next != nullptr)
			curr = curr->Next;

		curr->Next = newData;
	}

	m_mDataMutex.unlock();
}

// IN: void
// OUT: void
//
// Loops through the linked list and loads data that is pending to load to the GPU. This is only called
// by the GraphicsManager strictly.
void GraphicsManager::LoadPendingAssets()
{
	LoadedAssetData* stoppingAsset = m_lHead;
	
	// Loop through the list to find the end. Doing so will allow us to only process data we know about now
	// and not have to lock out the list for other threads. We will clean up the list later in on movement instead
	// to try to maintain a decent speed.
	while (stoppingAsset->Next != nullptr)
		stoppingAsset = stoppingAsset->Next;

	LoadedAssetData* curr = m_lHead;

	// Loop through our list to our stopping point
	bool run = true;
	while (run)
	{
		if (curr == stoppingAsset)
			run = false;

		char* data = curr->Data;
		char type = *data;

		// Check to see if we are a texture (0) or a mesh (1)
		if (type == 0)
		{
			DWORD sizeInBytes = *(DWORD*)(data + 1);
			UINT  imageID = *(UINT*)(data + 5);
			void* dataStartPoint = (void*)(data + 9);

			m_pResources->CreateImageFromMemory(imageID, dataStartPoint, (size_t)sizeInBytes);
		}
		else
		{
			bool finalize = *(data + 1) == 1 ? true : false;
			Mesh* mesh = nullptr;
			memcpy(&mesh, data + 2, sizeof(void*));
			
			if (mesh == nullptr)
				continue;

			for (size_t sMesh = 0; sMesh < mesh->GetSubMeshes().size(); sMesh++)
			{
				SubMesh &subMesh = mesh->GetSubMeshes()[sMesh];
				subMesh.startVertex = LoadVerts(&subMesh.vertices[0], subMesh.vertices.size());
				subMesh.startIndex = LoadIndices(&subMesh.indices[0], subMesh.indices.size());
				subMesh.indiceCount = (UINT)subMesh.indices.size();
				subMesh.vertexCount = (UINT)subMesh.vertices.size();
			}

			if (finalize)
				mesh->Finalize();
		}

		curr = curr->Next;
	}

	// Now that we have loaded our data we can lock the list and clean up anything needing clean up
	m_mDataMutex.lock();

	// Reset the current pointer to the head so we have our start point and set the head to our stopping point
	curr = m_lHead;
	m_lHead = stoppingAsset->Next;
	
	// We can now unlock the list as it is how we want it
	m_mDataMutex.unlock();

	// Now we need to loop through the data again and delete everything.
	run = true;
	while (run)
	{
		if (curr == stoppingAsset)
			run = false;

		LoadedAssetData* temp = curr;
		curr = curr->Next;

		delete[] temp->Data;
		delete temp;
	}
}

// IN: Mesh* - pointer to the mesh requesting verts
// OUT: vector<unsigned int> - List of offsets, size 0 on error
//
// Evaluates the mesh and available skinning verts and returns a list of offsets for writing
// each submesh to the skinning vertex buffer
std::vector<unsigned int> GraphicsManager::RequestSkinningVerts(Mesh* mesh)
{
	std::vector<unsigned int> offsets;

	if (mesh == nullptr)
		return offsets;

	unsigned int numSubMeshes = (unsigned int)mesh->GetSubMeshes().size();
	unsigned int totalVerts = 0;
	
	for (size_t subMesh = 0; subMesh < numSubMeshes; subMesh++)
		totalVerts += mesh->GetSubMeshes()[subMesh].vertexCount;

	// Find the first place in the vertex buffer that can support our total vert count
	size_t freeVertEntry = 0;
	for (; freeVertEntry < m_vVertexEntryList.size(); freeVertEntry++)
	{
		if (m_vVertexEntryList[freeVertEntry].totalVerts >= totalVerts)
			break;
	}

	// If we couldn't find an entry big enough
	if (freeVertEntry >= m_vVertexEntryList.size())
		return offsets;

	// If we did find an entry big enough, we can create our offsets to send back
	offsets.resize(numSubMeshes);

	// Set our first index to the start vertex, and then we can create the remainder
	offsets[0] = m_vVertexEntryList[freeVertEntry].startVertex;

	for (size_t subMesh = 1; subMesh < numSubMeshes; subMesh++)
		offsets[subMesh] = offsets[subMesh - 1] + mesh->GetSubMeshes()[subMesh - 1].vertexCount;

	// We need to trim the number of verts from our count so we know how many are still remaining in this entry
	m_vVertexEntryList[freeVertEntry].totalVerts -= totalVerts;

	// If we no longer have verts, remove us from the list, otherwise update our new start point
	if (m_vVertexEntryList[freeVertEntry].totalVerts == 0)
		m_vVertexEntryList.erase(m_vVertexEntryList.begin() + freeVertEntry);
	else
		m_vVertexEntryList[freeVertEntry].startVertex += totalVerts;

	return offsets;
}
// IN: Mesh*        - pointer to the mesh releasing verts
//     unsigned int - first offset of the lsit
// OUT: void
//
// Releases the vertices the mesh is writing so other meshes may use them
void GraphicsManager::ReleaseSkinningVerts(Mesh* mesh, unsigned int firstOffset)
{
	if (mesh == nullptr)
		return;

	unsigned int totalVerts = 0;
	unsigned int numSubMeshes = (unsigned int)mesh->GetSubMeshes().size();

	// Loop through our sub meshes to get our total vert count
	for (size_t subMesh = 0; subMesh < numSubMeshes; subMesh++)
		totalVerts += mesh->GetSubMeshes()[subMesh].vertexCount;

	// Create a nextIndex so we can look to see if we can combine with a node to our right
	unsigned int nextIndex = firstOffset + totalVerts;

	// Try to see if we can combine with any pre exisiting entries
	bool combinedLeft = false, combinedRight = false;
	for (size_t freeVertEntry = 0; freeVertEntry < m_vVertexEntryList.size(); freeVertEntry++)
	{
		if (combinedLeft && combinedRight)
			break;

		GraphicsManager::FreeVertexEntry& entry = m_vVertexEntryList[freeVertEntry];
		unsigned int entryNextIndex = entry.startVertex + entry.totalVerts;

		// Check if we can combine left
		if (entryNextIndex == firstOffset)
		{
			// If we can combine left, we need to update our first offset and total verts and pull the
			// entry out of the list
			totalVerts += entry.totalVerts;
			firstOffset = entry.startVertex;
			m_vVertexEntryList.erase(m_vVertexEntryList.begin() + freeVertEntry);
			freeVertEntry--;
			combinedLeft = true;
			continue;
		}
		// check if we can combine right
		else if (nextIndex == entry.startVertex)
		{
			// Update the entry based on our offset and total verts
			totalVerts += entry.totalVerts;
			m_vVertexEntryList.erase(m_vVertexEntryList.begin() + freeVertEntry);
			freeVertEntry--;
			combinedRight = true;
			continue;
		}
	}

	m_vVertexEntryList.push_back({ firstOffset, totalVerts });
}


// IN: size_t - index of image
//     size_t - slot for image
// OUT: bool - status of the texture assign
//
// Assigns the texture at the index to the passed in slot
bool GraphicsManager::SetTexture(ShaderType type, int index, int slot) const
{
	m_pPipelineManager->SetShaderResourceView(type, index, slot, m_pResources);

	return true;
}

// IN: void* - Pointer to the view matrix
// OUT: void
//
// Registered function to receive the updated view matrix when it changes
void GraphicsManager::UpdateViewMatrix(void* data)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(ViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, sizeof(Matrix4));
	m_pContext->Unmap(ViewBuffer, 0);

	//m_pContext->Map(BillboardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//memcpy(resource.pData, &CameraManager::Main()->transform->WorldMatrix(), sizeof(Matrix4));
	//m_pContext->Unmap(BillboardBuffer, 0);
}
// IN: void* - Pointer to the projeciton matrix
// OUT: void
//
// Registed function to receive the updated projection matrix when it changes
void GraphicsManager::UpdateProjectionMatrix(void* data)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(ProjectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, sizeof(Matrix4));
	m_pContext->Unmap(ProjectionBuffer, 0);
}

// IN: bool - Setting of Fullscreen
// OUT: void
//
// Sets the window to the desired setting
void GraphicsManager::SetFullscreen(bool fullscreen)
{
	if (!m_pSwapChain)
		return;

	// Set the new fullscreen state
	m_pSwapChain->SetFullscreenState(fullscreen, fullscreen ? m_pVideoOutput : NULL);
	
	if (fullscreen)
		SetGamma(m_fGamma);
}

// IN: UINT - width of the target
//     UINT - height of the target
// OUT: void
//
// Resizes the target to the passed in dimensions
void GraphicsManager::ResizeTarget(UINT width, UINT height)
{
	if (!m_pSwapChain)
		return;

	DXGI_SWAP_CHAIN_DESC desc;
	m_pSwapChain->GetDesc(&desc);

	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferDesc.RefreshRate.Numerator = 0;

	GetClosestOutputMode(desc.BufferDesc);

	if (!SUCCEEDED(m_pSwapChain->ResizeTarget(&desc.BufferDesc)))
	{
		PRINT("Failed to resize the target window!");
		return;
	}
}

// IN: void
// OUT: bool - Resize success
//
// Resize functions for the window
bool GraphicsManager::ResizeWindow()
{
	if (m_eGraphicsStatus != MANAGER_STATUS_INITIALIZED)
		return false;

	ID3D11RenderTargetView* nullRTV[1] = { nullptr };
	m_pContext->OMSetRenderTargets(1, nullRTV, nullptr);

	DX11RenderTargetView* pRTV = m_pResources->GetRenderTargetViewByID(0);
	SAFE_RELASE(pRTV->m_pRTV);

	EventSystem::GetInstance()->SendEventNow("ReleaseSwapChainResources", nullptr);

	HRESULT hr;
	if (!SUCCEEDED(hr = m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
	{
		ALERT("Error Resizing the Swap Chain!");
		PRINT("Error Resizing the Swap Chain!\n");
		return false;
	}

	EventSystem::GetInstance()->SendEventNow("SwapChainResized", m_pSwapChain);

	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTV->m_pRTV);

	D3D11_TEXTURE2D_DESC backBuffDesc;
	pBackBuffer->GetDesc(&backBuffDesc);
	SAFE_RELEASE(pBackBuffer);

	DX11DepthStencilView* pDSV = m_pResources->GetDepthStencilViewByID(0);
	DX11ShaderResourceView* pDSVSRV = m_pResources->GetShaderResourceByID(SHADER_RESOURCE_IMAGE, m_pResources->GetShaderResourceByName(SHADER_RESOURCE_IMAGE, L"DepthMap"));
	SAFE_RELASE(pDSV->m_pDSV);
	SAFE_RELASE(pDSVSRV->m_pSRV);

	// Recreate our Depth Stencil View
	ID3D11Texture2D* depthBuffer;
	D3D11_TEXTURE2D_DESC dBuff;
	ZeroMemory(&dBuff, sizeof(D3D11_TEXTURE2D_DESC));
	dBuff.Width = backBuffDesc.Width;
	dBuff.Height = backBuffDesc.Height;
	dBuff.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dBuff.ArraySize = 1;
	dBuff.MipLevels = 1;
	dBuff.Usage = D3D11_USAGE_DEFAULT;
	dBuff.SampleDesc.Count = 1;//(UINT)m_uiMSAACount;
	dBuff.SampleDesc.Quality = 0;//(UINT)m_iMSAAQuality;
	dBuff.Format = DXGI_FORMAT_R32_TYPELESS;

	hr = m_pDevice->CreateTexture2D(&dBuff, NULL, &depthBuffer);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Backbuffer Depth Stencil View!!"))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pDevice->CreateDepthStencilView(depthBuffer, &descDSV, &pDSV->m_pDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	m_pDevice->CreateShaderResourceView(depthBuffer, &srvDesc, &pDSVSRV->m_pSRV);
	SAFE_RELASE(depthBuffer);

	D3D11_VIEWPORT* viewport = m_pResources->GetViewportByID(0);
	viewport->Width = (float)backBuffDesc.Width;
	viewport->Height = (float)backBuffDesc.Height;

	struct Data { float Width, Height; };

	Data* pData = new Data;
	pData->Width = viewport->Width; pData->Height = viewport->Height;
	EventSystem::GetInstance()->SendEventNow("ResolutionChange", pData);

	return true;
}

// IN: width  - width in pixels of the screen
//     height - height in pixels of the screen
// OUT: bool - Resize success
//
// Resize functions for the window
bool GraphicsManager::ResizeWindow(float width, float height)
{
	if (m_eGraphicsStatus != MANAGER_STATUS_INITIALIZED)
		return false;

	ID3D11RenderTargetView* nullRTV[1] = { nullptr };
	m_pContext->OMSetRenderTargets(1, nullRTV, nullptr);

	DX11RenderTargetView* pRTV = m_pResources->GetRenderTargetViewByID(0);
	SAFE_RELASE(pRTV->m_pRTV);

	EventSystem::GetInstance()->SendEventNow("ReleaseSwapChainResources", nullptr);

	HRESULT hr = m_pSwapChain->ResizeBuffers(0, (UINT)width, UINT(height), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	EventSystem::GetInstance()->SendEventNow("SwapChainResized", m_pSwapChain);

	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTV->m_pRTV);

	D3D11_TEXTURE2D_DESC backBuffDesc;
	pBackBuffer->GetDesc(&backBuffDesc);
	SAFE_RELEASE(pBackBuffer);

	DX11DepthStencilView* pDSV = m_pResources->GetDepthStencilViewByID(0);
	DX11ShaderResourceView* pDSVSRV = m_pResources->GetShaderResourceByID(SHADER_RESOURCE_IMAGE, m_pResources->GetShaderResourceByName(SHADER_RESOURCE_IMAGE, L"DepthMap"));
	SAFE_RELASE(pDSV->m_pDSV);
	SAFE_RELASE(pDSVSRV->m_pSRV);

	// Recreate our Depth Stencil View
	ID3D11Texture2D* depthBuffer;
	D3D11_TEXTURE2D_DESC dBuff;
	ZeroMemory(&dBuff, sizeof(D3D11_TEXTURE2D_DESC));
	dBuff.Width = (UINT)width;
	dBuff.Height = (UINT)height;
	dBuff.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dBuff.ArraySize = 1;
	dBuff.MipLevels = 1;
	dBuff.Usage = D3D11_USAGE_DEFAULT;
	dBuff.SampleDesc.Count = 1;
	dBuff.SampleDesc.Quality = 0;
	dBuff.Format = DXGI_FORMAT_R32_TYPELESS;

	hr = m_pDevice->CreateTexture2D(&dBuff, NULL, &depthBuffer);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Backbuffer Depth Stencil View!!"))
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pDevice->CreateDepthStencilView(depthBuffer, &descDSV, &pDSV->m_pDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	m_pDevice->CreateShaderResourceView(depthBuffer, &srvDesc, &pDSVSRV->m_pSRV);
	SAFE_RELASE(depthBuffer);

	D3D11_VIEWPORT* viewport = m_pResources->GetViewportByID(0);
	viewport->Width = width;
	viewport->Height = height;

	struct Data { float Width, Height; };

	Data* pData = new Data;
	pData->Width = width; pData->Height = height;
	EventSystem::GetInstance()->SendEventNow("ResolutionChange", pData);

	PRINT("Resolution changed to " + std::to_string(width) + "x" + std::to_string(height) + ".\n");

	return true;
}

// IN: void
// OUT: bool - success of destroying the window
//
// Destroys our current window
bool GraphicsManager::DestroyWindow()
{
	//if (m_hWindow)
	//{
	//	// Clean up our window
	//	BOOL destoryed = ::DestroyWindow(m_hWindow);
	//	BOOL unregistered = UnregisterClass(m_pClassName, m_hInst);

	//	m_hWindow = nullptr;

	//	return true;
	//}

	if (m_pWindow)
	{
		SAFE_DELETE(m_pWindow);
		return true;
	}

	return false;
}

bool MergeCompare(const Renderer* left, const Renderer* right)
{
	if (left && right)
		return left->m_fDistFromCamera > right->m_fDistFromCamera;

	return false;
}

// IN: Void
// OUT: void
//
// Sorts all the blending renderers so blending is done properly
void GraphicsManager::SortBlendingRenderers(std::vector<Renderer*>& renderers)
{
	if (renderers.size() == 0 || CameraManager::Main() == nullptr)
		return;

	// Grab the main cameras position
	Vector3 camPos = CameraManager::Main()->transform->GetPosition();

	// Calculate the new distance from the camera for this frame
	auto iter = renderers.begin();
	auto end = renderers.end();
	for (; iter != end; ++iter)
		(*iter)->m_fDistFromCamera = ((*iter)->transform->GetPosition() - camPos).LengthSq();

	// Perform a merge sort on our renderers
	Inferno::MergeSort(renderers, MergeCompare);
}

#if defined(DEBUG) || defined(_DEBUG)
void GraphicsManager::StartEvent(const wchar_t* name)
{
	m_pAnnotation->BeginEvent(name);
}
void GraphicsManager::EndEvent()
{
	m_pAnnotation->EndEvent();
}
#endif

// IN: HINSTANCE - Instance of the application
//     WNDPROC   - Windows message processing function pointer
//     LPCWSTR   - Icon ID as string pointer  *OPTIONAL*
//     LPCWSTR   - Small Icon ID as string pointer *OPTIONAL*
//     LPCWSTR   - Menu name as string pointer *OPTIONAL*
//     LPCWSTR   - Class name as string pointer *OPTIONAL*
// OUT: bool - The success of initializing the manager
//
// Initializes the graphics manager
bool GraphicsManager::Initialize(HINSTANCE hInstance, IWndProc* WndProc, LPCWSTR IconName, LPCWSTR SmallIconName, LPCWSTR className)
{
	if (m_eGraphicsStatus == MANAGER_STATUS_INITIALIZED || m_eGraphicsStatus == MANAGER_STATUS_ERROR)
		return false;

	PRINT("Initializing Graphics Manger...\n");

	if (!InitializeDevice())
	{
		PRINT("FAILED to initize GrpahicsManager!\n");

		m_eGraphicsStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	m_pWindow = new Win32Window((int)m_fWidth, (int)m_fHeight, className);
	m_pWindow->Initialize(WndProc, IconName, SmallIconName);
	m_pWindow->SetWindowStyle(m_ePrevWindowStyle);

//	if (!CreateGameWindow())
//	{
//		PRINT("FAILED to initalize Graphics Manager!\n");
//
//		m_eGraphicsStatus = MANAGER_STATUS_ERROR;
//		return false;
//	}

	if (!InitializeDirectX())
	{
		PRINT("FAILED to initialize Graphics Manager!\n");

		m_eGraphicsStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	ResizeTarget((UINT)m_fWidth, (UINT)m_fHeight);
	SetFullscreen(m_ePrevWindowStyle == WINDOW_STYLE_FULLSCREEN ? true : false);

	m_pLightManager = new LightManager();

	if (!m_pLightManager->Initialize())
	{
		PRINT("FAILED to initialize Graphics Manager!\n");

		m_eGraphicsStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	m_pParticleManager = new ParticleManager();

	if (!m_pParticleManager->Initialize())
	{
		PRINT("FAILED to initialize Graphics Manager!\n");

		m_eGraphicsStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	m_pLightManager->SetAmbientColor(74.0f/255.0f, 78.0f/255.0f, 86.0f/255.0f);
	m_pLightManager->SetAmbientIntensity(1.0f);
	m_pRenderTree = new RenderTree(ROOT_LEVEL, AABB(Vector3::Zero(), Vector3(200.0f, 200.0f, 200.0f)));

	PRINT("Graphics Manager initialied Successfully!\n");

	// Now that everything has been initialized, we can set our gamma
	SetGamma(m_fGamma);

	m_eGraphicsStatus = MANAGER_STATUS_INITIALIZED;

	struct Data { float Width, Height; };

	Data* pData = new Data;
	pData->Width = m_fWidth; pData->Height = m_fHeight;
	EventSystem::GetInstance()->QueueEvent("ResolutionChange", pData);

	return true;
}

// IN: void
// OUT: void
//
// Handles any post Initialize operations
void GraphicsManager::PostInit()
{
	m_pParticleManager->PostInit();
}

// IN: void
// OUT: bool - Success of destroying to manager
//
// Destroys the current manager data
bool GraphicsManager::Destroy()
{
	if (m_eGraphicsStatus != MANAGER_STATUS_INITIALIZED)
		return false;

	PRINT("Destorying Grpahics Manager...");

	m_pSwapChain->SetFullscreenState(FALSE, NULL);

	m_fcGraphicsFlags->ClearFlags();

	SAFE_DELETE(m_pRenderTree);

	// Delete our managers
	SAFE_DELETE(m_pPipelineManager);
	SAFE_DELETE(m_pResources);
	SAFE_DELETE(m_pLightManager);
	SAFE_DELETE(m_pParticleManager);

	// Release our pointers
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);

#if defined(DEBUG) || defined(_DEBUG)
	SAFE_RELASE(m_pAnnotation);
#endif

	DestroyWindow();

	PRINT("Success!\n");

	m_eGraphicsStatus = MANAGER_STATUS_DESTORYED;
	return true;
}

// IN: Renderer* - Renderer to register
// OUT: void
//
// Registers the incoming renderer for rendering
void GraphicsManager::RegisterRenderer(Renderer* renderer)
{
	if (renderer->gameObject->GetName() == "IndicatorCircle")
		int debug = 0;

	if (!renderer->m_bAlphaBlending)
	{
		const Effect* effect = m_pPipelineManager->GetEffectByID(renderer->GetEffectID());
		DX11BlendState* blend = nullptr;

		if (effect)
			blend = m_pResources->GetBlendStateByID(effect->GetBlendStateID());

		if (blend)
			renderer->m_bAlphaBlending = blend->m_bAlphaBlending;
	}

#if (defined(DEBUG) || defined(_DEBUG)) && 1
	if (!renderer->m_bAlphaBlending)
	{
		auto iter = m_rRendererRegistery.GetRegisteredObjects().begin();
		for (; iter != m_rRendererRegistery.GetRegisteredObjects().end(); ++iter)
		{
			if (*iter == renderer)
			{
#if 1
				ASSERT(false, "Attempting to double register a renderer!");
				std::string doublereg = renderer->gameObject->GetName() + " is attempting to double register its renderer!\n";
				PRINT(doublereg.c_str());
#endif
				return;
			}
		}
	}
	else
	{
		auto iter = m_rAlphaRendererRegistery.GetRegisteredObjects().begin();
		for (; iter != m_rAlphaRendererRegistery.GetRegisteredObjects().end(); ++iter)
		{
			if (*iter == renderer)
			{
#if 1
				ASSERT(false, "Attempting to double register a renderer!");
				std::string doublereg = renderer->gameObject->GetName() + " is attempting to double register its renderer!\n";
				PRINT(doublereg.c_str());
#endif
				return;
			}
		}
	}
#endif

	if (!renderer->m_bAlphaBlending)
		m_rRendererRegistery.RegisterObject(renderer);
	else
		m_rAlphaRendererRegistery.RegisterObject(renderer);

	if (renderer->GetType() == SKINNED_MESH_RENDERER)
		m_rSkinnedRendererRegistry.RegisterObject((SkinnedMeshRenderer*)renderer);

#if !FRUSTUM_CULLING_DISABLED
	m_pRenderTree->insert(renderer);
#endif
}
// IN: UIRenderer* - Renderer to register
// OUT: void
//
// Registers the incoming renderer for rendering
void GraphicsManager::RegisterUIRenderer(UIRenderer* renderer)
{
	auto iter = m_rUIRegistry.m_vRegisteredObjects.begin();
	for (; iter != m_rUIRegistry.m_vRegisteredObjects.end(); ++iter)
	{
		if (renderer->GetLayer() < (*iter)->GetLayer()) {
			m_rUIRegistry.m_vRegisteredObjects.insert(iter, renderer);
			return;
		}
	}

	if (iter == m_rUIRegistry.m_vRegisteredObjects.end())
		m_rUIRegistry.m_vRegisteredObjects.push_back(renderer);
}

// IN: Light* - Pointer to the Light Component that needs to be registered
// OUT: bool - Whether or not the lComponent was able to be registered
// 
// Registers the given Light with the Light Manager. Returns true if it was registered, false otherwise.
bool GraphicsManager::RegisterLightComponent(Light* _lComponent)
{
	if (m_pLightManager)
	{
		m_pLightManager->RegisterLight(_lComponent);
		return true;
	}

	return false;
}

// IN: ParticleSystem* - Pointer to the Particle System Component that needs to be registered
// OUT: bool - Whether or not the Component was able to be registered
//
// Registers the given Particle System with the Particle Manager.
bool GraphicsManager::RegisterParticleSystem(ParticleSystem* _pComponent)
{
	if (m_pParticleManager)
	{
		m_pParticleManager->RegisterObject(_pComponent);
		return true;
	}

	return false;
}

// IN Renderer* - Renderer to unregister
// OUT: void
//
// Unregisters a renderer
void GraphicsManager::UnregisterRenderer(Renderer* renderer)
{
	if (renderer->gameObject->GetName() == "IndicatorCircle")
		int debug = 0;

	const Effect* effect = m_pPipelineManager->GetEffectByID(renderer->GetEffectID());
	DX11BlendState* blend = nullptr;

	if (effect)
		blend = m_pResources->GetBlendStateByID(effect->GetBlendStateID());

	if (blend && !blend->m_bAlphaBlending)
	{
		auto iter = m_rRendererRegistery.m_vRegisteredObjects.begin();
		for (; iter != m_rRendererRegistery.m_vRegisteredObjects.end(); ++iter)
		{
			if ((*iter) == renderer)
			{
				m_rRendererRegistery.m_vRegisteredObjects.erase(iter);
				break;
			}
		}
	}
	else
	{
		auto iter = m_rAlphaRendererRegistery.m_vRegisteredObjects.begin();
		for (; iter != m_rAlphaRendererRegistery.m_vRegisteredObjects.end(); ++iter)
		{
			if ((*iter) == renderer)
			{
				m_rAlphaRendererRegistery.m_vRegisteredObjects.erase(iter);
				break;
			}
		}
	}

	if (renderer->GetType() == SKINNED_MESH_RENDERER)
	{
		auto iter = m_rSkinnedRendererRegistry.m_vRegisteredObjects.begin();
		for (; iter != m_rSkinnedRendererRegistry.m_vRegisteredObjects.end(); ++iter)
		{
			if ((*iter) == renderer)
			{
				m_rSkinnedRendererRegistry.m_vRegisteredObjects.erase(iter);
				break;
			}
		}
	}

#if !FRUSTUM_CULLING_DISABLED
	m_pRenderTree->remove(renderer);
#endif
}
// IN UIRenderer* - Renderer to unregister
// OUT: void
//
// Unregisters a renderer
void GraphicsManager::UnregisterUIRenderer(UIRenderer* renderer)
{
	auto iter = m_rUIRegistry.m_vRegisteredObjects.begin();
	for (; iter != m_rUIRegistry.m_vRegisteredObjects.end(); ++iter)
	{
		if ((*iter) == renderer)
		{
			m_rUIRegistry.m_vRegisteredObjects.erase(iter);
			return;
		}
	}
}

//  IN : Light* - Pointer to the Light Component to unregistered
//  OUT : bool - Whether or not it was unregistered
// 
//  Unregisters the given Light with the Light Manager. Returns true if it was registered, false otherwise.
bool GraphicsManager::UnregisterLightComponent(Light* _lComponent)
{
	if (m_pLightManager)
	{
		m_pLightManager->UnregisterLight(_lComponent);
		return true;
	}

	return false;
}

// IN: ParticleSystem* - Pointer to the Particle System Component to unregister
// OUT: bool - Whether or not the Component was unregistered
//
// Uregisters the given Particle System with the Particle Manager.
bool GraphicsManager::UnregisterParticleSystem(ParticleSystem* _pComponent)
{
	if (m_pParticleManager)
	{
		m_pParticleManager->UnregisterObject(_pComponent);
		return true;
	}

	return false;
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
int GraphicsManager::CreateEffect(const char* name, int pixelShader, int rasterizerState, int blendState, int* samplerStates, int samplerCount)
{
	if (m_pPipelineManager)
		return m_pPipelineManager->CreateEffect(name, pixelShader, rasterizerState, blendState, samplerStates, samplerCount);

	return -1;
}

// IN: int                  - ID of the shader resource view
//     UINT                 - Size of each element in bytes
//     UINT                 - Flags to apply to the UAV
// OUT: int - ID of the created UAV, -1 on error
//
// Creates an unordered access view for the requested shader resource view
int GraphicsManager::CreateUnorderedAccessView(int textureID, UINT Flags, UINT format)
{
	return m_pResources->CreateUnorderedAccessViewFromTexture(textureID, Flags, (DXGI_FORMAT)format);
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
int GraphicsManager::CreateSolidColorImage(std::wstring name, UINT width, UINT height, UINT borderSize, Color backColor, Color borderColor)
{
	int texID = -1;

	// Make sure that the image we are trying to create is not too large
	if (width > 8192 || height > 8192)
		return texID;

	// Create an array large enough to hold all the colors for the texture
	Color* colors = new Color[width * height];

	// Fill out our array based on the incoming information
	for (UINT x = 0; x < width; x++)
	{
		for (UINT y = 0; y < height; y++)
		{
			if (x < borderSize || x >= width - borderSize ||
				y < borderSize || y >= height - borderSize)
				colors[x + y * width] = borderColor;
			else
				colors[x + y * width] = backColor;
		}
	}

	// Create our texture
	ID3D11Texture2D* pTexture = nullptr;
	D3D11_TEXTURE2D_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(D3D11_TEXTURE2D_DESC));
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDesc.MipLevels = 1;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = colors;
	data.SysMemPitch = sizeof(Color) * width;
	data.SysMemSlicePitch = 0;

	if (SUCCEEDED(m_pDevice->CreateTexture2D(&tDesc, &data, &pTexture)))
	{
		// Create the Shader Resource View
		ID3D11ShaderResourceView* SRV = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC sDesc;
		ZeroMemory(&sDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		sDesc.Format = tDesc.Format;
		sDesc.Texture2D.MipLevels = 1;
		sDesc.Texture2D.MostDetailedMip = 0;
		sDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		if (SUCCEEDED(m_pDevice->CreateShaderResourceView(pTexture, &sDesc, &SRV)))
			texID = AddImage(name, SRV);

		SAFE_RELASE(pTexture);
	}

	SAFE_ARRAY_DELETE(colors);
	return texID;
}

// IN: wstring - name of the image
//     void*   - pointer to a Shader Resource View
// OUT: int - ID of the Image added
//
// Adds the shader resource view to the active list of images
int GraphicsManager::AddImage(std::wstring name, void* image)
{
	return m_pResources->AddImage(name, image);
}

// IN: wstring  - Name for the ShaderResource
//     wstring* - Pointer to the array of image names
//     uint     - number of image names
// OUT: int - ID to the ShaderResourceView
int GraphicsManager::CreateTextureArray(std::wstring name, std::wstring* textureNames, UINT numTextures)
{
	return m_pResources->CreateTextureArray(name, textureNames, numTextures);
}

// IN: float - delta time
// OUT: bool - Success of the update
//
// Updates the current instance
bool GraphicsManager::Update()
{
	if (ASSERT(m_eGraphicsStatus != MANAGER_STATUS_INITIALIZED, L"Graphics Manager not initialized!"))
		return false;

	if (m_lHead != nullptr)
		LoadPendingAssets();

#if !FRUSTUM_CULLING_DISABLED
	std::vector<Renderer*> opagueRend, alphaRend;
	std::vector<SkinnedMeshRenderer*> skinnedRend;

	m_pRenderTree->Update(m_pRenderTree);
	m_pRenderTree->RetrieveVisibleObjects(opagueRend, skinnedRend, alphaRend, CameraManager::Main()->GetFrustum());
#endif

#pragma region Draw Frame
#if defined(DEBUG) || defined(_DEBUG)
	m_pSwapChain->Present(m_fcGraphicsFlags->GetFlagAsInt(VSYNC_ENABLED), 0);

	DXEvent DrawFame(m_pAnnotation, L"Draw Frame");
#endif

#pragma region PreDraw
	// Clear our render target from the previous frame
	m_pPipelineManager->SetDepthStencilView(0);
	m_pPipelineManager->ClearDepthStencilView(0, 1.0f, m_pResources);

	// Sort our blending renderers since they are now in their final position for rendering
#if FRUSTUM_CULLING_DISABLED
	SortBlendingRenderers(m_rAlphaRendererRegistery.m_vRegisteredObjects);
#else
	SortBlendingRenderers(alphaRend);
#endif
#pragma endregion

#pragma region Animation Pass
	{
#if defined(DEBUG) || defined(_DEBUG)
		DXEvent SO(m_pAnnotation, L"Animation Pass");
#endif

		// Update all the verts for skinned mesh renderers
		UINT stride = sizeof(Vertex), offset = 0;
		m_pContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
		m_pPipelineManager->SetPixelShader(-1, m_pResources);	// Turn off the pixel shader so we can use the StreamOutput
		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
#if FRUSTUM_CULLING_DISABLED
		auto iter = m_rSkinnedRendererRegistry.GetRegisteredObjects().begin();
		auto end = m_rSkinnedRendererRegistry.GetRegisteredObjects().end();
#else
		auto iter = skinnedRend.begin();
		auto end = skinnedRend.end();
#endif
		for (; iter != end; iter++)
		{
			SkinnedMeshRenderer* currRend = (*iter);

			if (!currRend->gameObject->ActiveInHierarchy() || !currRend->isEnabled() || !currRend->GetMesh() && !currRend->GetNumVerts())
				continue;

			currRend->PreSkinnedDraw();

			auto sIter = currRend->m_pMesh->GetSubMeshes().begin();
			auto sEnd = currRend->m_pMesh->GetSubMeshes().end();
			auto vIter = currRend->m_vSkinnedVertOffsets.begin();
			auto vEnd = currRend->m_vSkinnedVertOffsets.end();
			for (; sIter != sEnd && vIter != vEnd; sIter++, vIter++)
			{
				SubMesh subMesh = (*sIter);

				UINT offset[1] = { (*vIter) * (sizeof(Vertex)) };
				ID3D11Buffer* buffers[1] = { SkinnedVertexBuffer };
				m_pContext->SOSetTargets(1, buffers, offset);

				m_pPipelineManager->Draw(currRend->m_iStreamOutEffect, subMesh.startVertex, subMesh.vertexCount, m_pResources);

				buffers[0] = nullptr;
				offset[0] = 0;
				m_pContext->SOSetTargets(1, buffers, offset);
			}
		}

		// Remove the buffer from the StreamOutput stage so we can render from it
		UINT Offset[1] = { 0 };
		ID3D11Buffer* buffers[1] = { nullptr };
		m_pContext->SOSetTargets(1, buffers, Offset);
	}
#pragma endregion

#pragma region Opague Depth Pass
	{
#if FRUSTUM_CULLING_DISABLED
		auto iter = m_rRendererRegistery.GetRegisteredObjects().begin();
		auto end = m_rRendererRegistery.GetRegisteredObjects().end();
		auto aIter = m_rAlphaRendererRegistery.GetRegisteredObjects().begin();
		auto aEnd = m_rAlphaRendererRegistery.GetRegisteredObjects().end();
#else
		auto iter = opagueRend.begin();
		auto end = opagueRend.end();
		auto aIter = alphaRend.begin();
		auto aEnd = alphaRend.end();
#endif

		{
#if defined(DEBUG) || defined(_DEBUG)
			DXEvent DepthPass(m_pAnnotation, L"Opaque Depth Pass");
#endif
			// Do a depth pre-pass so we can do our Forward+ light culling
			int nullRTV[1] = { -1 };
			m_pPipelineManager->SetRenderTargetViews(1, nullRTV);
			m_pPipelineManager->SetDepthStencilView(m_iDepthStencilView);
			m_pPipelineManager->SetDepthStencilState(m_iDepthPrepassDepthStencilState, 0x00);
			m_pPipelineManager->SetInputLayout(m_iPositionOnlyInputLayout);
			m_pPipelineManager->SetVertexShader(m_iPositionOnlyVS, m_pResources);
			m_pPipelineManager->SetPixelShader(-1, m_pResources);
			m_pPipelineManager->SetRaterizerState(0);

			for (; iter != end; iter++)
			{
				// Grab the current renderer
				Renderer* currRend = (*iter);

				if (!currRend->gameObject->ActiveInHierarchy() || !currRend->isEnabled() || !currRend->GetMesh() && !currRend->GetNumVerts())
					continue;

				// Set the topology for the renderer
				switch (currRend->m_ttTopology)
				{
				case LINE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					break;
				case TRIANGLE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				case POINT_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					break;
				}

				// Updates buffers
				currRend->PreDraw();

				// Set any data that the renderer needs set up
				m_pPipelineManager->SetVertexBuffer(currRend->m_iVertexBuffer, 0, currRend->m_uiStride, 0);
				m_pPipelineManager->SetIndexBuffer(currRend->m_iIndexBuffer, DXGI_FORMAT_R32_UINT);
				m_pPipelineManager->SetGeometryShader(currRend->m_iGeometryShader, m_pResources);

				// Draw to the depth buffer
				m_pPipelineManager->ApplyDesiredStates(m_pResources);
				currRend->DepthPrePassDraw();
			}
		}
#pragma endregion

#pragma region Alpha Depth Pass
#if 0
		{
#if defined(DEBUG) || defined(_DEBUG)
			DXEvent DepthPass(m_pAnnotation, L"Alpha Depth Pass");
#endif

			m_pPipelineManager->SetInputLayout(m_iAlphaInputLayout);
			m_pPipelineManager->SetPixelShader(m_iAlphaTestPixelShader, m_pResources);
			m_pPipelineManager->SetVertexShader(m_iAlphaTestVertexShader, m_pResources);
			m_pPipelineManager->SetRaterizerState(3);
			m_pPipelineManager->SetDepthStencilState(-1, 0);

			for (; aIter != aEnd; aIter++)
			{
				// Grab the current renderer
				Renderer* currRend = (*aIter);

				if (!currRend->gameObject->ActiveInHierarchy() || !currRend->isEnabled() || !currRend->GetMesh() && !currRend->GetNumVerts())
					continue;

				// Set the topology for the renderer
				switch (currRend->m_ttTopology)
				{
				case LINE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					break;
				case TRIANGLE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				case POINT_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					break;
				}

				// Updates buffers
				currRend->PreDraw();

				// Set any data that the renderer needs set up
				m_pPipelineManager->SetVertexBuffer(currRend->m_iVertexBuffer, 0, currRend->m_uiStride, 0);
				m_pPipelineManager->SetIndexBuffer(currRend->m_iIndexBuffer, DXGI_FORMAT_R32_UINT);
				m_pPipelineManager->SetGeometryShader(currRend->m_iGeometryShader, m_pResources);

				// Draw to the depth buffer
				m_pPipelineManager->ApplyDesiredStates(m_pResources);
				currRend->DepthPrePassDraw();
			}
		}
#endif
#pragma endregion

#pragma region Light Pass
		// Perform the light culling if we have any lights in the scene
		m_pLightManager->Update();
#pragma endregion

#pragma region Opague Rendering
		{
#if defined(DEBUG) || defined(_DEBUG)
			DXEvent MeshDraw(m_pAnnotation, L"Opaque Render Draws");
#endif
			// Set the render target back
			m_pPipelineManager->SetRenderTargetViews(1, &m_iRenderTarget);

#if FRUSTUM_CULLING_DISABLED
			for (iter = m_rRendererRegistery.GetRegisteredObjects().begin(); iter != end; iter++)
#else
			for (iter = opagueRend.begin(); iter != end; iter++)
#endif
			{
				// Grab the current renderer
				Renderer* currRend = (*iter);

				if (!currRend->gameObject->ActiveInHierarchy() || !currRend->isEnabled() || !currRend->GetMesh() && !currRend->GetNumVerts())
					continue;

				currRend->PreDraw();

				// Set the topology for the renderer
				switch (currRend->m_ttTopology)
				{
				case LINE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					break;
				case TRIANGLE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				case POINT_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					break;
				}

				// Set any data that the renderer needs set up
				m_pPipelineManager->SetVertexBuffer(currRend->m_iVertexBuffer, 0, currRend->m_uiStride, 0);
				m_pPipelineManager->SetIndexBuffer(currRend->m_iIndexBuffer, DXGI_FORMAT_R32_UINT);
				m_pPipelineManager->SetInputLayout(currRend->m_iInputLayout);
				m_pPipelineManager->SetVertexShader(currRend->m_iVertexShader, m_pResources);
				m_pPipelineManager->SetGeometryShader(currRend->m_iGeometryShader, m_pResources);
				m_pPipelineManager->SetDepthStencilState(currRend->m_iDepthStencilState, 1);

				currRend->Draw();
			}
		}
#pragma endregion

#pragma region Clear Screen
		{
#if defined(DEBUG) || defined(_DEBUG)
			DXEvent ClearScreen(m_pAnnotation, L"Render Skybox");
#endif
			// Use the main camera to clear out any pixels at this point that haven't been written too
			CameraManager::ClearTargetView();
		}
#pragma endregion

#pragma region Alpha Rendering
		// Draw all Alpha Renderers
		{
#if defined(DEBUG) || defined(_DEBUG)
			DXEvent SpritePass(m_pAnnotation, L"Alpha Render Draws");
#endif

#if FRUSTUM_CULLING_DISABLED
			for (aIter = m_rAlphaRendererRegistery.GetRegisteredObjects().begin(); aIter != aEnd; aIter++)
#else
			for (aIter = alphaRend.begin(); aIter != aEnd; aIter++)
#endif
			{
				// Grab the current renderer
				Renderer* currRend = (*aIter);

				if (!currRend->gameObject->ActiveInHierarchy() || !currRend->isEnabled() || !currRend->GetMesh() && !currRend->GetNumVerts())
					continue;

				currRend->PreDraw();

				// Set the topology for the renderer
				switch (currRend->m_ttTopology)
				{
				case LINE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					break;
				case TRIANGLE_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				case POINT_TOPOLOGY:
					m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					break;
				}

				// Set any data that the renderer needs set up
				m_pPipelineManager->SetVertexBuffer(currRend->m_iVertexBuffer, 0, currRend->m_uiStride, 0);
				m_pPipelineManager->SetIndexBuffer(currRend->m_iIndexBuffer, DXGI_FORMAT_R32_UINT);
				m_pPipelineManager->SetInputLayout(currRend->m_iInputLayout);
				m_pPipelineManager->SetVertexShader(currRend->m_iVertexShader, m_pResources);
				m_pPipelineManager->SetGeometryShader(currRend->m_iGeometryShader, m_pResources);
				m_pPipelineManager->SetDepthStencilState(m_iAlphaDepthStencilState, 1);

				currRend->Draw();
			}

			//auto iter = m_rSpriteRenderer.m_vRegisteredObjects.begin();
			//auto end = m_rSpriteRenderer.m_vRegisteredObjects.end();
			//for (; iter != end; iter++)
			//{
			//	// Grab the current renderer
			//	SpriteRenderer* currRend = (*iter);

			//	if (!currRend->gameObject->ActiveInHierarchy() || !currRend->isEnabled() || !currRend->GetMesh() && !currRend->GetNumVerts())
			//		continue;

			//	currRend->PreDraw();

			//	// Set the topology for the renderer
			//	switch (currRend->m_ttTopology)
			//	{
			//	case LINE_TOPOLOGY:
			//		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			//		break;
			//	case TRIANGLE_TOPOLOGY:
			//		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//		break;
			//	case POINT_TOPOLOGY:
			//		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			//		break;
			//	}

			//	// Set any data that the renderer needs set up
			//	m_pPipelineManager->SetVertexBuffer(currRend->m_iVertexBuffer, 0, currRend->m_uiStride, 0);
			//	m_pPipelineManager->SetIndexBuffer(currRend->m_iIndexBuffer, DXGI_FORMAT_R32_UINT);
			//	m_pPipelineManager->SetInputLayout(currRend->m_iInputLayout);
			//	m_pPipelineManager->SetVertexShader(currRend->m_iVertexShader, m_pResources);
			//	m_pPipelineManager->SetGeometryShader(currRend->m_iGeometryShader, m_pResources);
			//	m_pPipelineManager->SetDepthStencilState(currRend->m_iDepthStencilState, 1);

			//	currRend->Draw();
			//}
		}
#pragma endregion
	}

#pragma region Particle Pass
	{
#if defined(DEBUG) || defined(_DEBUG)
		DXEvent ParticlePass(m_pAnnotation, L"Particle Pass");
#endif

		// Emit any particles that have acculated per emitter
		m_pParticleManager->Emit();
		// Update any alive particles and create a render list of active particles
		m_pParticleManager->Simulate();
		// After everything has been drawn, draw our particles
		m_pParticleManager->Render();
	}
#pragma endregion

#pragma region UI Pass
	{
#if defined(DEBUG) || defined(_DEBUG)
		DXEvent UIPass(m_pAnnotation, L"Render UI");
#endif

		// Set up the pipeline for drawling UI elements
		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_pPipelineManager->SetInputLayout(UIInputID);
		m_pPipelineManager->SetIndexBuffer(m_pResources->GetBufferByName(INDEX_BUFFER, L"IndexBuffer"), DXGI_FORMAT_R32_UINT);
		m_pPipelineManager->SetVertexShader(UIVertexID, m_pResources);
		m_pPipelineManager->SetGeometryShader(UIGeometryID, m_pResources);
		m_pPipelineManager->SetDepthStencilView(-1); // Turn off the Depth Stencil View for rendering UI
		m_pPipelineManager->SetDepthStencilState(-1, 0xFF);

		{
			auto iter = m_rUIRegistry.GetRegisteredObjects().begin();
			auto end = m_rUIRegistry.GetRegisteredObjects().end();
			for (; iter != end; iter++)
			{
				UIRenderer* currRend = (*iter);

				if (!currRend->IsActiveInHierachry())
					continue;

				currRend->PreDraw();

				//m_pPipelineManager->SetShaderResourceView(PIXEL_SHADER, currRend->GetTexture()->GetImage(), 0, m_pResources); ** MOVED TO PREDRAW **
#if defined(DEBUG) || defined(_DEBUG)
				m_pAnnotation->BeginEvent(Inferno::ToWideString(currRend->GetUIElement()->GetName()).c_str());
#endif

				m_pPipelineManager->DrawIndexed(currRend->m_uiEffectID, 0, 1, m_pResources);

#if defined(DEBUG) || defined(_DEBUG)
				m_pAnnotation->EndEvent();
#endif
			}
		}
	}
#pragma endregion

#if !defined(DEBUG) && !defined(_DEBUG)
	m_pSwapChain->Present(m_fcGraphicsFlags->GetFlagAsInt(VSYNC_ENABLED), 0);
#endif
#pragma endregion

	return true;
}

// IN: Font*       - Pointer to the font to use
//     wstring     - string of text to render
//     D2D1_RECT_F - Drawling rectangle region
// OUT: void
//
// Adds the string and region to a list for the font type to render
void GraphicsManager::RenderText(Font* font, std::wstring text, D2D_RECT_F* rectRegion)
{
	m_vRenderStrings.push_back(std::tuple<Font*, std::wstring, D2D_RECT_F>(font, text, *rectRegion));
}

// IN: void
// OUT: void
//
// Updates the window if any of the window settings changed
void GraphicsManager::WindowUpdate()
{
	//UINT width = (UINT)m_fWidth;
	//UINT height = (UINT)m_fHeight;
	//UINT windowFlags = 0;

	//// Grabs the desktop dimensions
	//RECT desktop;
	//GetWindowRect(GetDesktopWindow(), &desktop);

	//// Set up the flags for the window based on our style of window
	//switch (m_eWindowStyle)
	//{
	//case WINDOW_STYLE_FULLSCREEN:
	//case WINDOW_STYLE_WINDOWED:
	//{
	//	windowFlags = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

	//	// Set up a default window Rect
	//	RECT ClientRect = { 0, 0, width, height };
	//	RECT WindowRect = { 0, 0, 0, 0 };
	//	// Adjust our windows rect based on the set flags
	//	AdjustWindowRect(&WindowRect, windowFlags, FALSE);

	//	int windowWidth = (WindowRect.right - WindowRect.left) + ClientRect.right;
	//	int windowHeight = (WindowRect.bottom - WindowRect.top) + ClientRect.bottom;
	//	int left = (desktop.right - windowWidth) >> 1;
	//	int top = (desktop.bottom - windowHeight) >> 1;

	//	BOOL success = SetWindowLong(m_hWindow, GWL_STYLE, windowFlags);
	//	success = SetWindowPos(m_hWindow, 0, left, top, windowWidth, windowHeight, SWP_SHOWWINDOW);
	//	success = success;
	//}
	//break;
	//// If we are borderless, get the desktop width and height to use as the windows width and height
	//case WINDOW_STYLE_BORDERLESS:
	//{
	//	m_fWidth = (float)desktop.right;
	//	m_fHeight = (float)desktop.bottom;
	//	m_fAspectRatio = m_fHeight / m_fWidth;
	//	windowFlags = WS_POPUP;

	//	SetWindowLong(m_hWindow, GWL_STYLE, windowFlags);
	//	SetWindowPos(m_hWindow, 0, 0, 0, (int)m_fWidth, (int)m_fHeight, SWP_SHOWWINDOW);
	//}
	//break;
	//}

	ResizeWindow();
}

// IN: void
// OUT: void
//
// Accept changes that have been made to the window
void GraphicsManager::ApplyWindowChanges()
{
	if (m_pWindow)
		m_ePrevWindowStyle = m_pWindow->GetWindowStyle();
	m_fPrevWidth = m_fWidth;
	m_fPrevHeight = m_fHeight;
}

// IN: void
// OUT: void
//
// Revert changes made to the board if ApplyWindowChanges hasn't been called
void GraphicsManager::RevertWindowChanges()
{
	if (m_pWindow)
		m_pWindow->SetWindowStyle(m_ePrevWindowStyle);
	m_fWidth = m_fPrevWidth;
	m_fHeight = m_fPrevHeight;

	m_pWindow->SetSize((int)m_fWidth, (int)m_fHeight);
	SetFullscreen(m_ePrevWindowStyle == WINDOW_STYLE_FULLSCREEN ? true : false);
	ResizeWindow(m_fWidth, m_fHeight);
}

// IN: void
// OUT: void
//
// Applies the current pipeline state
void GraphicsManager::ApplyPipelineState()
{
	m_pPipelineManager->ApplyDesiredStates(m_pResources);
}

// IN: UINT  - ID of buffer to update
//     void* - pointer to the data to update
//     UINT  - size of data in bytes
// OUT: void
//
// Updates the sub resource requested if the resource is a valid buffer
void GraphicsManager::UpdateSubResource(UINT ID, void* data, UINT size)
{
	m_pResources->UpdateSubResource(ID, data, size);
}

// IN: int - ID of the buffer to copy to
//     int - offset to use
//     int - ID of the UVA that has a counter
// OUT: void
//
// Copies the invisible counter on the UAV to the buffer
void GraphicsManager::CopyStructureCount(int bufferID, int offset, int UAVID)
{
	m_pResources->CopyStructureCount(bufferID, offset, UAVID);
}

// Draw calls that don't use the pipeline manager
// ------------------------------------------------------------------------------------------

// IN: unsigned int - Number of Vertices to render
//     unsigned int - Start Vertex
// OUT: void
//
// Draws the vertices with the current pipeline setup
void GraphicsManager::Draw(unsigned int numVertices, unsigned int startVertex)
{
	m_pContext->Draw(numVertices, startVertex);
}

// IN: unsigned int - Number of Indices to render
//     unsigned int - Start Index
//     unsigned int - Start Vertex
// OUT: void
//
// Draws the indices using the ccurrent pipeline setup
void GraphicsManager::DrawIndexed(unsigned int numIndices, unsigned int startIndex, unsigned int startVertex)
{
	m_pContext->DrawIndexed(numIndices, startIndex, startVertex);
}

//--------------------------------------------------------------------------------------------

// IN: int   - ID of the effect to use
//     Mesh* - Pointer to the mesh
// OUT: void
//
// Draws the mesh using the requested effect
void GraphicsManager::Draw(int EffectID, Mesh* mesh)
{
	if (!mesh)
		return;

	auto subMeshes = mesh->GetSubMeshes();
	for (size_t sMesh = 0; sMesh < subMeshes.size(); sMesh++)
		m_pPipelineManager->DrawIndexed(EffectID, subMeshes[sMesh].startIndex, subMeshes[sMesh].indiceCount, subMeshes[sMesh].startVertex, m_pResources);
}
// IN: int - ID of the effect to use
//     int - Start Vertex
//     int - Num Vertices
// OUT: void
//
// Calls a standard draw call using the passed in effect ID
void GraphicsManager::Draw(int EffectID, int StartVertex, int NumVerts)
{
	m_pPipelineManager->Draw(EffectID, StartVertex, NumVerts, m_pResources);
}
// IN: int          - ID of the effect to use
//     unsigned int - start index
//     unsinged int - num indices
//     unsigned int - start vertex
// OUT: void
//
// Draws the passed in mesh using the effect requested
void GraphicsManager::DrawIndexed(int EffectID, unsigned int startIndex, unsigned int numIndices, unsigned int startVertex)
{
	m_pPipelineManager->DrawIndexed(EffectID, startIndex, numIndices, startVertex, m_pResources);
}
// IN: int  - ID of the buffer with the draw arguments
//     UINT - offset in bytes for the draw arguments
// OUT: void
//
// Applies changes to the pipeline and uses the buffer from the passed in ID to draw
void GraphicsManager::DrawIndirect(int BufferID, UINT offset)
{
	m_pPipelineManager->DrawIndirect(BufferID, offset, m_pResources);
}
// IN: unsigned int - X thread groups
//     unsigned int - Y thread groups
//     unsigned int - Z thread groups
// OUT: void
//
// Calls dispatch on the current pipeline
void GraphicsManager::Disptach(unsigned int x, unsigned int y, unsigned int z)
{
	m_pPipelineManager->Dispatch(x, y, z, m_pResources);
}

void GraphicsManager::SetAmbientColor(float r, float g, float b)
{
	m_pLightManager->SetAmbientColor(r, g, b);
}
void GraphicsManager::SetAmbientColor(Vector3 color)
{
	m_pLightManager->SetAmbientColor(color);
}
void GraphicsManager::SetAmbientColor(Vector4 color)
{
	m_pLightManager->SetAmbientColor(color);
}
void GraphicsManager::SetAmbientIntensity(float intensity)
{
	m_pLightManager->SetAmbientIntensity(intensity);
}

// IN: ShaderType - type of shader to grab
//     wstring    - name of shader
// OUT: int - ID of shader, -1 on error
//
// Returns the ID of requested shader if it exists
int GraphicsManager::GetShader(ShaderType type, std::wstring name)
{
	return m_pResources->GetShaderByName(type, name);
}
// IN: wstring - name of buffer
// OUT: int - ID of buffer, -1 on error
//
// Returns the ID of requested buffer if it exists
int GraphicsManager::GetConstantBuffer(std::wstring name)
{
	return m_pResources->GetConstantBufferByName(name);
}
// IN: BufferType - type of buffer
//     wstring    - name of buffer
// OUT: int - ID of buffer, -1 on error
//
// Returns the ID of requested buffer if it exists
int GraphicsManager::GetBuffer(BufferType type, std::wstring name)
{
	return m_pResources->GetBufferByName(type, name);
}
// IN: wstring - name of image
// OUT: int - ID of image, -1 on error
//
// Returns the ID of requested image if it exists
int GraphicsManager::GetImage(std::wstring name)
{
	return m_pResources->GetImageByName(name);
}
// IN: SHADER_RESOURCE_TYPE - type of resource to get
//     wstring              - name of Shader Resource
// OUT: int - ID of Shader Resource
//
// Returns the requested shader resource ID
int GraphicsManager::GetShaderResource(SHADER_RESOURCE_TYPE type, std::wstring name)
{
	return m_pResources->GetShaderResourceByName(type, name);
}
// IN: wstring - name of Unordered Access View
// OUT: int - ID of Unordered Access View
//
// Returns the requested Unordered Access View ID
int GraphicsManager::GetUnorderedAccessView(std::wstring name)
{
	return m_pResources->GetUnorderedAccessViewByName(name);
}
// IN: wstring - name of input layout
// OUT: int - ID of input layout, -1 on error
//
// Returns the ID of requested input layout if it exists
int GraphicsManager::GetInputLayout(std::wstring name)
{
	return m_pResources->GetInputLayoutByName(name);
}
// IN: wstring - name of render target view
// OUT: int - ID of render target view, -1 on error
//
// Returns the ID of requested render target view if it exists
int GraphicsManager::GetRenderTargetView(std::wstring name)
{
	return m_pResources->GetRenderTargetViewByName(name);
}
// IN: wstring - name of depth stencil view
// OUT: int - ID of depth stencil view, -1 on error
//
// Returns the ID of requested depth stencil view if it exists
int GraphicsManager::GetDepthStencilView(std::wstring name)
{
	return m_pResources->GetDepthStencilViewByName(name);
}
// IN: wstring - name of sampler state
// OUT: int - ID of sampler state, -1 on error
//
// Returns the ID of requested sampler state if it exists
int GraphicsManager::GetSamplerState(std::wstring name)
{
	return m_pResources->GetSamplerStateByName(name);
}
// IN: wstring - name of rasterizer state
// OUT: int - ID of rasterizer state, -1 on error
//
// Returns the ID of requested rasterizer state if it exists
int GraphicsManager::GetRasterizerState(std::wstring name)
{
	return m_pResources->GetRasterizerStateByName(name);
}
// IN: wstring - name of blend state
// OUT: int - ID of the blend state, -1 on error
//
// Returns the ID of the requested blend state if it exists
int GraphicsManager::GetBlendState(std::wstring name)
{
	return m_pResources->GetBlendStateByName(name);
}
// IN: wstring - name of the depth stencil state
// OUT: int - ID of the depth stencil state, -1 on error
//
// Returns the ID of the requested depth stencil state if it exists
int GraphicsManager::GetDepthStencilState(std::wstring name)
{
	return m_pResources->GetDepthStencilStateByName(name);
}

// IN: void
// OUT: void
//
// Destroys all active particles
void GraphicsManager::CleanUpParticles()
{
	m_pParticleManager->CleanupParticles();
}

// IN: Matrix4 - World Matrix to apply
// OUT: void
//
// Applies the passed in world matrix to the world buffer
void GraphicsManager::SetWorldMatrix(const Matrix4& world)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = m_pContext->Map(WorldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &world, sizeof(Matrix4));
	m_pContext->Unmap(WorldBuffer, 0);
}
// IN: Matrix4 - View Matrix to apply
// OUT: void
//
// Applies the passed in view matrix to the view buffer
void GraphicsManager::SetViewMatrix(const Matrix4& view)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(ViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &view, sizeof(Matrix4));
	m_pContext->Unmap(ViewBuffer, 0);
}
// IN: Matrix4 - Projection Matrix to apply
// OUT: void
//
// Applies the passed in projection matrix to the projection buffer
void GraphicsManager::SetProjectionMatrix(const Matrix4& projection)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(ProjectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &projection, sizeof(Matrix4));
	m_pContext->Unmap(ProjectionBuffer, 0);
}
// IN: COLOR - Color to apply
// OUT: void
//
// Applies the passed in color to the color buffer
void GraphicsManager::SetColor(const Color& color)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(ColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &color, sizeof(Color));
	m_pContext->Unmap(ColorBuffer, 0);
}
// IN: Vector2 - UV Offset to apply
// OUT: void
//
// Applies the passd in uv offsets to the offset buffer
void GraphicsManager::SetUVOffsets(const Vector2& offset, const Vector2& tiling)
{
	struct UVBuffer
	{
		Vector2 Offset;
		Vector2 Tiling;
	};

	UVBuffer buffer = { offset, tiling };

	D3D11_MAPPED_SUBRESOURCE resource;
	m_pContext->Map(UVOffsetBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &buffer, sizeof(UVBuffer));
	m_pContext->Unmap(UVOffsetBuffer, 0);
}

// IN: unsigned int - ID of the layout
// OUT: void
//
// Assigns the requested input layout to the InputAssembler
void GraphicsManager::SetInputLayout(unsigned int ID)
{
	m_pPipelineManager->SetInputLayout(ID);
}
// IN: ShaderType - type of shader to apply
//     int        - ID to the shader being applied
// OUT: void
//
// Sets the current shader type at the passed in ID to the pipeline
void GraphicsManager::SetShader(ShaderType type, int ID)
{
	switch (type)
	{
	case VERTEX_SHADER:
		m_pPipelineManager->SetVertexShader(ID, m_pResources);
		break;
	case GEOMETRY_SHADER:
		m_pPipelineManager->SetGeometryShader(ID, m_pResources);
		break;
	case PIXEL_SHADER:
		m_pPipelineManager->SetPixelShader(ID, m_pResources);
		break;
	case COMPUTE_SHADER:
		m_pPipelineManager->SetComputeShader(ID, m_pResources);
		break;
	}
}
// IN: ShaderType    - type of shader to apply to
//     unsigned int  - starting slot
//     unsigned int  - total slots used
//     ID3D11Buffer* - pointer to the starting buffer
// OUT: void
//
// Applies all the passed in buffers to the pipeline
void GraphicsManager::SetConstantBuffers(ShaderType type, unsigned int startSlot, unsigned int totalBuffers, ID3D11Buffer** startBuffer)
{
	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		m_pContext->VSSetConstantBuffers(startSlot, totalBuffers, startBuffer);
		break;
	case ShaderType::GEOMETRY_SHADER:
		m_pContext->GSSetConstantBuffers(startSlot, totalBuffers, startBuffer);
		break;
	case ShaderType::PIXEL_SHADER:
		m_pContext->PSSetConstantBuffers(startSlot, totalBuffers, startBuffer);
		break;
	case ShaderType::COMPUTE_SHADER:
		m_pContext->CSSetConstantBuffers(startSlot, totalBuffers, startBuffer);
		break;
	default:
		break;
	}
}
// IN: ShaderType   - Type of Shader to apply to
//     unsigned int - ID of the constant buffer
//     unsigned int - Slot of the buffer
// OUT: void
//
// Assigns the passed in constant buffer to the pipeline
void GraphicsManager::SetConstantBuffer(ShaderType type, unsigned int ID, unsigned int Slot)
{
	m_pPipelineManager->SetConstantBuffer(type, ID, Slot, m_pResources);
}
// IN: unsigned int - ID of the buffer
//     unsigned int - stride of the buffer in bytes
//     unsigned int - offset of the buffer in bytes
// OUT: void
//
// Applies the passed in vertex buffer to the InputAssembler
void GraphicsManager::SetVertexBuffer(unsigned int ID, unsigned int stride, unsigned int offset)
{
	m_pPipelineManager->SetVertexBuffer(ID, 0, stride, offset);
}
// IN: unsigned int - ID of the buffer
// OUT: void
//
// Applies the passed in index buffer to the InputAssembler
void GraphicsManager::SetIndexBuffer(unsigned int ID)
{
	m_pPipelineManager->SetIndexBuffer(ID, DXGI_FORMAT_R32_UINT);
}
// IN: ShaderType - Shader to apply UAV to
//     int        - UAV ID
//     int        - Slot to apply
// OUT: void
//
// Applies a Unordered Access View to the requested slot of the requested shader type
void GraphicsManager::SetUnorderedAccessView(ShaderType type, int ID, int Slot, int Count)
{
	m_pPipelineManager->SetUnorderedAccessView(type, ID, Slot, m_pResources, Count);
}
// IN: ShaderType - type of shader to apply to
//     int        - ID of the sampler to apply
//     int        - Slot for the sampler
// OUT: void
//
// Applies all the passed in states to the pipeline
void GraphicsManager::SetSamplerStates(ShaderType type, int ID, int Slot)
{
	m_pPipelineManager->SetSamplerState(type, ID, Slot, m_pResources);
}
// IN: ShaderType                - type of shader to apply to
//     unsigned int              - starting slot
//     unsigned int		         - total slots used
//     ID3D11ShaderResourceView* - pointer to the starting view
// OUT: void
//
// Applies all the passed in views to the pipeline
void GraphicsManager::SetShaderResourceViews(ShaderType type, unsigned int startSlot, unsigned int totalViews, ID3D11ShaderResourceView** startView)
{
	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		m_pContext->VSSetShaderResources(startSlot, totalViews, startView);
		break;
	case ShaderType::GEOMETRY_SHADER:
		m_pContext->GSSetShaderResources(startSlot, totalViews, startView);
		break;
	case ShaderType::PIXEL_SHADER:
		m_pContext->PSSetShaderResources(startSlot, totalViews, startView);
		break;
	case ShaderType::COMPUTE_SHADER:
		m_pContext->CSSetShaderResources(startSlot, totalViews, startView);
		break;
	default:
		break;
	}
}

// IN: TopologyType - Topology to set
// OUT: void
//
// Applies the requested topology
void GraphicsManager::SetTopology(TopologyType topology)
{
	switch (topology)
	{
	case LINE_TOPOLOGY:
		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case TRIANGLE_TOPOLOGY:
		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case POINT_TOPOLOGY:
		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case LINESTRIP_TOPOLOGY:
		m_pPipelineManager->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		break;
	}
}
// IN: int - ID of Blend State
// OUT: void
//
// Updates the desired state with incoming information
void GraphicsManager::SetBlendState(int ID)
{
	m_pPipelineManager->SetBlendState(ID);
}
// IN: int - ID of the Rasterizer State
// OUT: void
//
// Updates the desired state with the incoming information
void GraphicsManager::SetRasterizerState(int ID)
{
	m_pPipelineManager->SetRaterizerState(ID);
}
// IN: int  - ID of the Depth Stencil State
//     UINT - Reference value for the stencil
// OUT: void
//
// Assigns the requested depth stencil state and ref value to the pipeline
void GraphicsManager::SetDepthStencilState(int ID, UINT ref)
{
	m_pPipelineManager->SetDepthStencilState(ID, ref);
}

// IN: int - ID of the Depth Stencil View
// OUT: void
//
// Assigns the requested depth stencil view to the pipeline
void GraphicsManager::SetDepthStencilView(int ID)
{
	m_pPipelineManager->SetDepthStencilView(ID);
}

// IN: char* - name of the effect
// OUT: int - ID of the effect
//
// Returns the ID for the effect of the corresonding name if it exists
int GraphicsManager::GetEffectByName(const char* name) const
{
	return m_pPipelineManager->GetEffectByName(name);
}

void GraphicsManager::SetLineAA(bool lineAA) { m_fcGraphicsFlags->SetFlag(ANTI_ALIASING_ENABLED, lineAA); }
void GraphicsManager::SetVSync(bool vsync) { m_fcGraphicsFlags->SetFlag(VSYNC_ENABLED, vsync); }
void GraphicsManager::SetWindowInfo(WINDOW_STYLE style, float width, float height)
{
	SetWindowDimensions(width, height);
	SetWindowStyle(style);
}
void GraphicsManager::SetWindowStyle(WINDOW_STYLE style)
{
	if (m_pWindow)
	{
		m_ePrevWindowStyle = m_pWindow->GetWindowStyle();
		m_pWindow->SetWindowStyle(style);

		ResizeTarget((UINT)m_fWidth, (UINT)m_fHeight);
		SetFullscreen((style == WINDOW_STYLE_FULLSCREEN) ? true : false);
	}
	else
		m_ePrevWindowStyle = style;
}
void GraphicsManager::SetWindowDimensions(float width, float height)
{
	if (width < 1 || height < 1)
		return;

	if (width != m_fWidth && height != m_fHeight)
	{
		m_fPrevWidth = m_fWidth;
		m_fWidth = width;
		m_fPrevHeight = m_fHeight;
		m_fHeight = height;
	}

	m_pWindow->SetSize((int)m_fWidth, (int)m_fHeight);
	ResizeWindow(m_fWidth, m_fHeight);
	ResizeTarget((UINT)m_fWidth, (UINT)m_fHeight);
}
void GraphicsManager::SetMSAACount(unsigned int count)
{
	if (count > MAX_MSAA_COUNT)
		count = MAX_MSAA_COUNT;
	else if (count < MIN_MSAA_COUNT)
		count = MIN_MSAA_COUNT;

	m_uiMSAACount = count;
}
void GraphicsManager::SetMSAAQuality(int quality)
{
	if (quality > MIN_MSAA_QUALITY)
		quality = MIN_MSAA_QUALITY;
	else if (quality < MAX_MSAA_QUALITY)
		quality = MAX_MSAA_QUALITY;

	m_iMSAAQuality = quality;
}
void GraphicsManager::SetAnisotropicFiltering(unsigned int anisotropic)
{
	if (anisotropic < MIN_ANISOTROPIC_COUNT)
		anisotropic = MIN_ANISOTROPIC_COUNT;
	else if (anisotropic > MAX_ANISOTROPIC_COUNT)
		anisotropic = MAX_ANISOTROPIC_COUNT;

	m_uiAnisotropicCount = anisotropic;

	if (m_eGraphicsStatus == MANAGER_STATUS_INITIALIZED)
		EventSystem::GetInstance()->SendEventNow("AnisotropicChanged", &m_uiAnisotropicCount);
}
void GraphicsManager::SetGamma(float gamma)
{
	DXGI_GAMMA_CONTROL GammaControl;
	float newGamma = Clamp(MIN_GAMMA, MAX_GAMMA, gamma);

	if (m_pWindow && m_pWindow->GetWindowStyle() == WINDOW_STYLE_FULLSCREEN)
	{
		DXGI_GAMMA_CONTROL_CAPABILITIES cap;
		IDXGIOutput* currOutput = nullptr;

		// Grab the current output we are on
		HRESULT hr = m_pSwapChain->GetContainingOutput(&currOutput);
		hr = m_pVideoOutput->GetGammaControlCapabilities(&cap);

		for (UINT i = 0; i < cap.NumGammaControlPoints; i++)
		{
			float curveValue = min(1.0f, i * gamma / (cap.NumGammaControlPoints - 1));

			GammaControl.GammaCurve[i].Red = curveValue;
			GammaControl.GammaCurve[i].Green = curveValue;
			GammaControl.GammaCurve[i].Blue = curveValue;
		}

		GammaControl.Scale.Red = 1.0f;//newGamma;
		GammaControl.Scale.Green = 1.0f;//newGamma;
		GammaControl.Scale.Blue = 1.0f;//newGamma;

		GammaControl.Offset.Red = 0.0f;
		GammaControl.Offset.Green = 0.0f;
		GammaControl.Offset.Blue = 0.0f;

		m_pVideoOutput->SetGammaControl(&GammaControl);
	}

	m_fGamma = gamma;
}

HWND GraphicsManager::Window() const { return m_pWindow ? m_pWindow->GetHWnd() : nullptr; }
bool GraphicsManager::IsWindowDirty() const{ return !(m_fWidth == m_fPrevWidth && m_fHeight == m_fPrevHeight && (m_pWindow ? m_pWindow->GetWindowStyle() : WINDOW_STYLE_WINDOWED) == m_ePrevWindowStyle); }
bool GraphicsManager::VSyncEnabled() const { return m_fcGraphicsFlags->GetFlag(VSYNC_ENABLED); }
WINDOW_STYLE GraphicsManager::GetWindowStyle() const { return m_pWindow ? m_pWindow->GetWindowStyle() : WINDOW_STYLE_WINDOWED; }
float GraphicsManager::GetWindowWidth() const { return m_fWidth; }
float GraphicsManager::GetWindowHeight() const { return m_fHeight; }
float GraphicsManager::GetAspectRatio() const { return m_fAspectRatio; }
unsigned int GraphicsManager::GetMSAACount() const { return (unsigned int)m_uiMSAACount; }
int GraphicsManager::GetMSAAQuality() const { return m_iMSAAQuality; }
bool GraphicsManager::AntiAliasingEnabled() const { return m_fcGraphicsFlags->GetFlag(ANTI_ALIASING_ENABLED); }
unsigned int GraphicsManager::GetAnisotropicFiltering() const { return (unsigned int)m_uiAnisotropicCount; }
bool GraphicsManager::MSAAEnabled() const { return m_fcGraphicsFlags->GetFlag(MSAA_ENBALED); }
float GraphicsManager::GetGamma() const
{
	//float gamma = 0.0f;

	if (m_pVideoOutput)
	{
		//DXGI_GAMMA_CONTROL GammaControl;
		//m_pVideoOutput->GetGammaControl(&GammaControl);
		//gamma = GammaControl.Scale.Red;
	}

	return m_fGamma;
}
UINT GraphicsManager::GetNumDisplays() const
{
	UINT displays = 0;
	IDXGIDevice* pDXGIDevice;
	HRESULT hr = S_OK;

	// Try to get the DXGI Device from our device
	if (!SUCCEEDED(hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice)))
		return displays;

	// Try to get the DXGI Adapter from the DXGI Device
	IDXGIAdapter* pAdapter;
	hr = pDXGIDevice->GetAdapter(&pAdapter);
	SAFE_RELASE(pDXGIDevice);

	if (!SUCCEEDED(hr))
		return displays;

	IDXGIOutput* pOutput;
	while (pAdapter->EnumOutputs(displays, &pOutput) != DXGI_ERROR_NOT_FOUND)
	{
		SAFE_RELASE(pOutput);
		displays++;
	}
	SAFE_RELASE(pAdapter);

	return displays;
}
void GraphicsManager::GetSupportedResolutions(std::vector<std::wstring>& Resolutions, UINT displayID) const
{
	Resolutions.clear();
	Resolutions.reserve(20);
	IDXGIDevice* pDXGIDevice;
	HRESULT hr = S_OK;
	
	// Try to get the DXGI Device from our device
	if (!SUCCEEDED(hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice)))
		return;

	// Try to get the DXGI Adapter from the DXGI Device
	IDXGIAdapter* pAdapter;
	hr = pDXGIDevice->GetAdapter(&pAdapter);
	SAFE_RELASE(pDXGIDevice);

	if (!SUCCEEDED(hr))
		return;

	IDXGIOutput* pOutput;
	hr = pAdapter->EnumOutputs(displayID, &pOutput);
	SAFE_RELASE(pAdapter);
	if (!SUCCEEDED(hr))
		return;

	UINT num = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;

	pOutput->GetDisplayModeList(format, flags, &num, 0);

	DXGI_MODE_DESC* desc = new DXGI_MODE_DESC[num];
	pOutput->GetDisplayModeList(format, flags, &num, desc);
	std::set<std::wstring> existingResolutions;

	for (UINT i = 0; i < num; i++)
	{
		if (desc[i].Width < 640 || desc[i].Height < 480 ||
			desc[i].Width > HIGHEST_SUPPORTED_RES_X || desc[i].Height > HIGHEST_SUPPORTED_RES_Y)
			continue;

		std::wstring resolution = std::to_wstring(desc[i].Width) + L"x" + std::to_wstring(desc[i].Height);

		if (existingResolutions.find(resolution) == existingResolutions.end())
		{
			Resolutions.push_back(resolution);
			existingResolutions.insert(resolution);
		}
	}

	SAFE_RELASE(pOutput);
	SAFE_ARRAY_DELETE(desc);

	return;
}