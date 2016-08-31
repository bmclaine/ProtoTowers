// Author: Jonathan Gyurkovics
//
// Handles making view and projection matrices for displaying

#pragma once

#include "IComponent.h"
#include "MathLib.h"
#include "Physics\Collision.h"
#include "Color.h"

class Mesh;

typedef enum ClearType {
	CLEAR_TO_COLOR = 0,
	CLEAR_TO_SKYBOX = 1,
};

class Camera : public IComponent
{
private:
	Matrix4 m_m4ViewMatrix;			// The view matrix of the camera
	Matrix4 m_m4ProjectionMatrix;	// The projection matrix of the camera
	float m_fAspectRatio;			// Holds a copy of the aspect ratio
	float m_fZNear;					// Holds the cameras ZNear
	float m_fZFar;					// Holds the cameras ZFar
	float m_fFOV;					// Holds the cameras FOV in radians
	float m_fDistToPlayer;			// Holds the maximum distance to the player
	ClearType m_ctType;				// Holds the clear mode of the camera
	Color m_cClearColor;			// Holds the clear color if we are in Clear to Color mode
	int m_iSkyboxImage;				// Holds the image for the skybox
	bool m_bProjectionDirty;		// Lets the camera manger know that the projection matrix is dirty
	bool m_bColliding;				// holds if the camera is colliding

	// IN: void
	// OUT: void
	//
	// Updates the projection matrix
	void UpdateProjection();

	virtual ~Camera() override;

	virtual IComponent& operator=(IComponent& _rhs) override;
	Camera& operator=(const Camera& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	Camera(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	//virtual void OnTriggerEnter(Collider * _col) override;

	//virtual void OnTriggerExit(Collider * _col) override;

	//virtual void Update() override;

	// IN: void
	// OUT: void
	//
	// Update that is called every frame
	void UpdateCamera();

	// IN: int   - ID of effect to use
	//     Mesh* - pointer to the cube mesh
	// OUT: void
	//
	// Clears the screen using the passed in effect and cube mesh
	void ClearView(int EffectID, Mesh* mesh);

	void SetCameraData(float aspect, float znear, float zfar, float fov);
	void SetAspect(float aspect);
	void SetZNear(float znear);
	void SetZFar(float zfar);
	void SetFOV(float fov);
	void SetClearType(ClearType type);
	void SetSkyboxImage(int image);
	void SetClearColor(Color color);

	Matrix4 GetViewMatrix() const;
	Matrix4 GetProjectionMatrix() const;
	ClearType GetClearType() const;
	Frustum GetFrustum() const;
	
	friend class GameObject;
	friend class CameraManager;
};


