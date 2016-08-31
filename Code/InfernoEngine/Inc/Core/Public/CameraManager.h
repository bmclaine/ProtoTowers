// Author: Jonathan Gyurkovics
//
// Handles updating only cameras who are active and handles sending out events when the main camera updates

#pragma once

#include "ShaderInfo.h"

class Camera;
class Mesh;

class CameraManager
{
private:
	static Camera* s_pMainCamera;
	static int s_iInputLayout;
	static TopologyType s_ttTopology;
	static int s_iVertexBuffer;
	static int s_iStride;
	static int s_iIndexBuffer;
	static int s_iVertexShader;
	static int s_iGeometryShader;
	static int s_iCameraSkyboxEffect;
	static int s_iCameraColorEffect;
	static int s_iDepthStencilState;
	static Mesh* s_pSkyboxMesh;

	void Initialize();
	void PostInit();

	void Update();

	CameraManager();
	~CameraManager();

public:
	static void SetMainCamera(Camera* camera);
	static Camera* Main();

	static void ClearTargetView();

	friend class InfernoEngine;
};

