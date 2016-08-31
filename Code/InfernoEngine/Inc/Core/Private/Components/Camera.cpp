#include "../Public/Components/Camera.h"
#include "Components\Transform.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Physics\Collider.h"
#include "GameObject.h"
#include "InfernoTime.h"
#include "../../Interface/Public/Engine.h"
#include "GlobalIncludes.h"
#include "CameraManager.h"
#include "AudioManager.h"
#include "Physics\CollisionLibrary.h"

Camera::Camera(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_m4ViewMatrix = Matrix4::Identity();
	m_bProjectionDirty = true;
	m_cClearColor = Color(0.05f, 0.2f, 0.1f, 1.0f);
	m_ctType = CLEAR_TO_COLOR;
	m_fDistToPlayer = 2.5f * 2.5f;
	SetCameraData(800.0f / 600.0f, 0.1f, 1000.0f, ToRadians(65.0f));
	m_fDistToPlayer = -1.0f;
	m_bColliding = false;
}
Camera::~Camera() {}

IComponent& Camera::operator=(IComponent& _rhs)
{
	Camera* rhsCamera = dynamic_cast<Camera*>(&_rhs);
	if (rhsCamera == nullptr) {
		return *this;
	}

	return operator=(*rhsCamera);
}

Camera& Camera::operator=(const Camera& _rhs)
{
	// === Copy over the Members
	m_fFOV = _rhs.m_fFOV;
	m_fZNear = _rhs.m_fZNear;
	m_fZFar = _rhs.m_fZFar;
	m_fAspectRatio = _rhs.m_fAspectRatio;
	m_m4ProjectionMatrix = _rhs.m_m4ProjectionMatrix;
	m_fcFlags = _rhs.m_fcFlags;

	m_bProjectionDirty = true;

	return *this;
}
IComponent* Camera::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	Camera* newCam = new Camera(gameObject, transform);
	(*newCam) = *this;

	return newCam;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void Camera::Init()
{
	//AudioManager::Instance()->PostEvent(3897924658U);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Camera::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "AspectRatio";
	Property* prop = data->GetProperty(propertyName);
	//m_fAspectRatio = (float)atof(prop->value.c_str());

	/*propertyName = "ZNear";
	prop = data->GetProperty(propertyName);
	m_fZNear = (float)atof(prop->value.c_str());

	propertyName = "ZFar";
	prop = data->GetProperty(propertyName);
	m_fZFar = (float)atof(prop->value.c_str());

	propertyName = "FOV";
	prop = data->GetProperty(propertyName);
	m_fFOV = (float)atof(prop->value.c_str());*/

	propertyName = "DistanceToPlayer";
	prop = data->GetProperty(propertyName);
	m_fDistToPlayer = (float)atof(prop->value.c_str());

	propertyName = "ClearType";
	prop = data->GetProperty(propertyName);
	m_ctType = (ClearType)atoi(prop->value.c_str());

	propertyName = "ClearColor";
	prop = data->GetProperty(propertyName);
	std::vector<std::string> colors = Inferno::StringParse(prop->value, ',');
	m_cClearColor.Red = (float)atof(colors[0].c_str());
	m_cClearColor.Green = (float)atof(colors[1].c_str());
	m_cClearColor.Blue = (float)atof(colors[2].c_str());
	m_cClearColor.Alpha = (float)atof(colors[3].c_str());

	propertyName = "SkyBoxImage";
	prop = data->GetProperty(propertyName);
	m_iSkyboxImage = Inferno::GetImage(Inferno::ToWideString(prop->value).c_str());

	propertyName = "IsMain";
	prop = data->GetProperty(propertyName);
	bool isMain = atoi(prop->value.c_str()) != 0;

	if (isMain){
		CameraManager::SetMainCamera(this);
	}

	UpdateProjection();
}

// IN: void
// OUT: void
//
// Update that is called every frame
void Camera::UpdateCamera()
{
	//if (transform->WorldUpdated())
		m_m4ViewMatrix = transform->InvWorldMatrix();
}

// IN: void
// OUT: void
//
// Updates the projection matrix
void Camera::UpdateProjection()
{
	using namespace DirectX;

	XMStoreFloat4x4(&m_m4ProjectionMatrix, XMMatrixPerspectiveFovLH(m_fFOV, m_fAspectRatio, m_fZNear, m_fZFar));
}

// IN: int   - ID of effect to use
//     Mesh* - pointer to the cube mesh
// OUT: void
//
// Clears the screen using the passed in effect and cube mesh
void Camera::ClearView(int EffectID, Mesh* mesh)
{
	if (m_ctType == CLEAR_TO_COLOR)
		InfernoEngine_Graphics::SetColor(m_cClearColor);
	else
		InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_iSkyboxImage, 0);

	InfernoEngine_Graphics::Draw(EffectID, mesh);
}



void Camera::SetCameraData(float aspect, float znear, float zfar, float fov)
{
	m_fAspectRatio = aspect;
	m_fZNear = znear;
	m_fZFar = zfar;
	m_fFOV = fov;
	m_bProjectionDirty = true;

	UpdateProjection();
}
void Camera::SetAspect(float aspect)
{
	m_fAspectRatio = aspect;
	m_bProjectionDirty = true;

	UpdateProjection();
}
void Camera::SetZNear(float znear)
{
	m_fZNear = znear;
	m_bProjectionDirty = true;

	UpdateProjection();
}
void Camera::SetZFar(float zfar)
{
	m_fZFar = zfar;
	m_bProjectionDirty = true;

	UpdateProjection();
}
void Camera::SetFOV(float fov)
{
	m_fFOV = fov;
	m_bProjectionDirty = true;

	UpdateProjection();
}
void Camera::SetClearType(ClearType type) { m_ctType = type; }
void Camera::SetSkyboxImage(int image) { m_iSkyboxImage = image; }
void Camera::SetClearColor(Color color) { m_cClearColor = color; }

Matrix4 Camera::GetViewMatrix() const { return m_m4ViewMatrix; }
Matrix4 Camera::GetProjectionMatrix() const { return m_m4ProjectionMatrix; }
ClearType Camera::GetClearType() const { return m_ctType; }
Frustum Camera::GetFrustum() const
{
	if (this == nullptr)
		return Frustum();

	Frustum viewFrustum;
	CollisionLibrary::BuildFrustum(viewFrustum, m_fFOV, m_fZNear, m_fZFar, m_fAspectRatio, transform->WorldMatrix());

	return viewFrustum;
}