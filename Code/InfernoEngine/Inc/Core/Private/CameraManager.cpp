#include "CameraManager.h"
#include "EventSystem.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "../../Interface/Public/Engine.h"

#include "Components\Camera.h"
#include "Components\Transform.h"

#include "Assets\Vertex.h"

Camera* CameraManager::s_pMainCamera = nullptr;
int CameraManager::s_iInputLayout = -1;
TopologyType CameraManager::s_ttTopology = TopologyType::TRIANGLE_TOPOLOGY;
int CameraManager::s_iVertexBuffer = -1;
int CameraManager::s_iStride = sizeof(Vertex);
int CameraManager::s_iIndexBuffer = -1;
int CameraManager::s_iVertexShader = -1;
int CameraManager::s_iGeometryShader = -1;
int CameraManager::s_iCameraSkyboxEffect = -1;
int CameraManager::s_iCameraColorEffect = -1;
int CameraManager::s_iDepthStencilState = -1;
Mesh* CameraManager::s_pSkyboxMesh = nullptr;

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
	s_pSkyboxMesh = nullptr;
}

void CameraManager::Initialize()
{
	s_iInputLayout = InfernoEngine_Graphics::GetInputLayout(L"LineLayout");		// Line layout uses only position data, which we want
	s_iVertexBuffer = InfernoEngine_Graphics::GetBuffer(VERTEX_BUFFER, L"VertexBuffer");
	s_iIndexBuffer = InfernoEngine_Graphics::GetBuffer(INDEX_BUFFER, L"IndexBuffer");
	s_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"SkyboxVertexShader");
	s_iDepthStencilState = InfernoEngine_Graphics::GetDepthStencilState(L"SkyboxStencilState");

	int SamplerState = InfernoEngine_Graphics::GetSamplerState(L"WrapSamplerState");
	s_iCameraSkyboxEffect = Inferno::CreateEffect("CameraSkyboxEffect", InfernoEngine_Graphics::GetShader(PIXEL_SHADER, L"SkyboxPixelShader"),
		InfernoEngine_Graphics::GetRasterizerState(L"SkyboxRasterizerState"), InfernoEngine_Graphics::GetBlendState(L"NoBlendingBlendState"), &SamplerState, 1);
	s_iCameraColorEffect = Inferno::CreateEffect("CameraColorEffect", InfernoEngine_Graphics::GetShader(PIXEL_SHADER, L"ColorOnlyPixelShader"),
		InfernoEngine_Graphics::GetRasterizerState(L"SkyboxRasterizerState"), InfernoEngine_Graphics::GetBlendState(L"NoBlendingBlendState"), &SamplerState, 1);
}

void CameraManager::PostInit()
{
	s_pSkyboxMesh = Inferno::GetMeshByName("Cube");
}

void CameraManager::Update()
{
	if (s_pMainCamera == nullptr)
		return;

	// Check to see if we need to update our projection matrix
	if (s_pMainCamera->m_bProjectionDirty)
	{
		EventSystem::GetInstance()->SendEventNow("MAIN_CAMERA_PROJECTION_UPDATE", (void*)&s_pMainCamera->GetProjectionMatrix());
		s_pMainCamera->m_bProjectionDirty = false;
	}

	// Update only the main camera, since there is no need to update non active cameras
	s_pMainCamera->UpdateCamera();

	if (!ASSERT(s_pMainCamera->transform->WorldMatrix().IsValid(), "Camera Matrix is invalid!"))
		return;

	// Inform anything that needs to know about the view matrix if there was an update to it
	//if (s_pMainCamera->transform->WorldUpdated())
		EventSystem::GetInstance()->SendEventNow("MAIN_CAMERA_VIEW_UPDATE", (void*)&s_pMainCamera->GetViewMatrix());
}

void CameraManager::ClearTargetView()
{
	if (s_pMainCamera == nullptr)
		return;

	InfernoEngine_Graphics::SetInputLayout(s_iInputLayout);
	InfernoEngine_Graphics::SetTopology(s_ttTopology);
	InfernoEngine_Graphics::SetVertexBuffer(s_iVertexBuffer, s_iStride, 0);
	InfernoEngine_Graphics::SetIndexBuffer(s_iIndexBuffer);
	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, s_iVertexShader);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);		// Make sure the geometry shader is turned off
	InfernoEngine_Graphics::SetDepthStencilState(s_iDepthStencilState, 1);
	
	if (s_pMainCamera->GetClearType() == CLEAR_TO_SKYBOX)
		s_pMainCamera->ClearView(s_iCameraSkyboxEffect, s_pSkyboxMesh);
	else
		s_pMainCamera->ClearView(s_iCameraColorEffect, s_pSkyboxMesh);
}

void CameraManager::SetMainCamera(Camera* camera) { s_pMainCamera = camera; }
Camera* CameraManager::Main() { return s_pMainCamera; }