#include "LightManager.h"
#include "Components\Light.h"
#include "Components\Transform.h"

#include "EventSystem.h"
#include "../../Interface/Public/ModuleInterface.h"

LightManager::LightManager() : m_rLightRegistry(200)
{
	m_WindowInformation.numLights = 0;

	m_vLightWorldPositions.resize(200);
	m_vPointLights.resize(200);
	m_vLightColors.resize(200);
}

LightManager::~LightManager()
{
	EventSystem::GetInstance()->UnregisterFromEvent("ResolutionChange", this, BindEventFunction(&LightManager::OnResolutionChange, this));
	EventSystem::GetInstance()->UnregisterFromEvent("MAIN_CAMERA_VIEW_UPDATE", this, BindEventFunction(&LightManager::OnViewChange, this));
	EventSystem::GetInstance()->UnregisterFromEvent("MAIN_CAMERA_PROJECTION_UPDATE", this, BindEventFunction(&LightManager::OnProjectionChange, this));
}

bool LightManager::Initialize()
{
	m_iComputeShader = InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"LightComputeShader");
	m_iLightPositionResource = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"LightCentersAndRadii");
	m_iLightPositionBuffer = InfernoEngine_Graphics::GetBuffer(STRUCTURED_BUFFER, L"LightCentersAndRadii");
	m_iLightIndicesUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"LightIndiceLists");
	m_iLightIndicesResource = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"LightIndiceLists");
	m_iLightColorAndIntensityResource = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"LightColorAndIntensity");
	m_iLightColorAndIntensityBuffer = InfernoEngine_Graphics::GetBuffer(STRUCTURED_BUFFER, L"LightColorAndIntensity");
	m_iDepthTexture = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_IMAGE, L"DepthMap");
	m_iWindowInformationBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"WindowInformation");
	m_iInverseProjectionBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"InverseProjection");
	m_iWorldViewBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"WorldView");
	m_iAmbientColorBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"Ambience");

	EventSystem::GetInstance()->RegisterForEvent("ResolutionChange", this, BindEventFunction(&LightManager::OnResolutionChange, this));
	EventSystem::GetInstance()->RegisterForEvent("MAIN_CAMERA_VIEW_UPDATE", this, BindEventFunction(&LightManager::OnViewChange, this));
	EventSystem::GetInstance()->RegisterForEvent("MAIN_CAMERA_PROJECTION_UPDATE", this, BindEventFunction(&LightManager::OnProjectionChange, this));

	return true;
}
bool LightManager::Destroy()
{
	return true;
}

void LightManager::Update()
{
	if (NUM_CELLS < m_uiNumXTiles * m_uiNumYTiles)
		return;

	InfernoEngine_Graphics::UpdateSubResource(m_iAmbientColorBuffer, &m_f4AmbientLightAndIntensity, sizeof(Vector4));

	// Update all of our light information incase any of the lights changed this frame
	size_t i = 0;
	for (; i < m_WindowInformation.numLights; i++)
	{
		Light* currLight = m_rLightRegistry.GetRegisteredObjects()[i];

		/*m_vLightWorldPositions[i]*/ m_vPointLights[i].Position = currLight->transform->GetPosition();
		m_vPointLights[i].Radius = currLight->GetRadius();
		m_vLightColors[i].Color = currLight->GetColor();
		m_vLightColors[i].Intensity = currLight->GetIntensity();
	}

	if (m_WindowInformation.numLights)
	{
		//Vec3TransformCoordArray(&m_vPointLights[0].Position, sizeof(PointLight), &m_vLightWorldPositions[0], sizeof(Vector3), m_WindowInformation.numLights, &m_m4ViewMatrix);
		InfernoEngine_Graphics::UpdateSubResource(m_iLightPositionBuffer, &m_vPointLights[0], sizeof(PointLight) * m_WindowInformation.numLights);
		InfernoEngine_Graphics::UpdateSubResource(m_iLightColorAndIntensityBuffer, &m_vLightColors[0], sizeof(LightColor) * m_WindowInformation.numLights);
	}

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(L"Light Culling");
#endif

	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 2);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 3);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 4);
	InfernoEngine_Graphics::SetDepthStencilView(-1); // Turn off the Depth Stencil View so we can assign the texture to the compute shader
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_iComputeShader);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iLightPositionResource, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iDepthTexture, 1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iLightIndicesUAV, 0);
	
	InfernoEngine_Graphics::Disptach(m_uiNumXTiles, m_uiNumYTiles, 1);

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 1);
	InfernoEngine_Graphics::ApplyPipelineState();
	InfernoEngine_Graphics::SetDepthStencilView(0);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_iLightIndicesResource, 2);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_iLightPositionResource, 3);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_iLightColorAndIntensityResource, 4);

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

void LightManager::SetAmbientColor(float r, float g, float b)
{
	m_f4AmbientLightAndIntensity.x = r;
	m_f4AmbientLightAndIntensity.y = g;
	m_f4AmbientLightAndIntensity.z = b;
}
void LightManager::SetAmbientColor(Vector3 color)
{
	m_f4AmbientLightAndIntensity.x = color.x;
	m_f4AmbientLightAndIntensity.y = color.y;
	m_f4AmbientLightAndIntensity.z = color.z;
}
void LightManager::SetAmbientColor(Vector4 color)
{
	m_f4AmbientLightAndIntensity.x = color.x;
	m_f4AmbientLightAndIntensity.y = color.y;
	m_f4AmbientLightAndIntensity.z = color.z;
}
void LightManager::SetAmbientIntensity(float intensity) { m_f4AmbientLightAndIntensity.w = intensity; }

Vector3 LightManager::GetAmbientColor() const { return m_f4AmbientLightAndIntensity; }
float LightManager::GetAmbientIntensity() const { return m_f4AmbientLightAndIntensity.w; }

void LightManager::OnResolutionChange(void* _pData)
{
	m_WindowInformation.width = *(static_cast<float*>(_pData));
	m_WindowInformation.height = *((static_cast<float*>(_pData)+1));

	InfernoEngine_Graphics::UpdateSubResource(m_iWindowInformationBuffer, &m_WindowInformation, sizeof(WindowInformation));

	m_uiNumXTiles = unsigned int((m_WindowInformation.width + TILE_X_SIZE - 1) / (float)TILE_X_SIZE);
	m_uiNumYTiles = unsigned int((m_WindowInformation.height + TILE_Y_SIZE - 1) / (float)TILE_Y_SIZE);
}

void LightManager::OnViewChange(void* pData)
{
	m_m4ViewMatrix = *(Matrix4*)pData;
	InfernoEngine_Graphics::UpdateSubResource(m_iWorldViewBuffer, pData, sizeof(Matrix4));
}
void LightManager::OnProjectionChange(void* pData)
{
	Matrix4 mat = static_cast<Matrix4*>(pData)->Inverse();

	InfernoEngine_Graphics::UpdateSubResource(m_iInverseProjectionBuffer, &mat, sizeof(Matrix4));
}

void LightManager::RegisterLight(Light* lComponent)
{
	m_rLightRegistry.RegisterObject(lComponent);
	m_WindowInformation.numLights = (unsigned int)m_rLightRegistry.GetRegisteredObjects().size();

	// Resize our vectors if we have more lights then we can currently hold
	if (m_vLightWorldPositions.capacity() < m_WindowInformation.numLights)
	{
		m_vLightWorldPositions.resize(m_vLightWorldPositions.capacity() * 2);
		m_vPointLights.resize(m_vPointLights.capacity() * 2);
		m_vLightColors.resize(m_vLightColors.capacity() * 2);
	}

	InfernoEngine_Graphics::UpdateSubResource(m_iWindowInformationBuffer, &m_WindowInformation, sizeof(WindowInformation));
}
void LightManager::UnregisterLight(Light* lComponent)
{
	if (m_rLightRegistry.UnregisterObject(lComponent))
	{
		m_WindowInformation.numLights = (unsigned int)m_rLightRegistry.GetRegisteredObjects().size();

		InfernoEngine_Graphics::UpdateSubResource(m_iWindowInformationBuffer, &m_WindowInformation, sizeof(WindowInformation));
	}
}