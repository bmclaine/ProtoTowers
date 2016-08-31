#pragma once

#define MAX_LIGHTS 3000
#define MAX_LIGHTS_PER_CELL 50
#define NUM_CELLS 12000 //8100			// basically 16x16 pixel cells at 1080p
#define TILE_X_SIZE 16
#define TILE_Y_SIZE 16

#include "Registry.h"
#include "MathLib.h"

class Light;

class LightManager
{
private:
	struct WindowInformation
	{
		float width;
		float height;
		unsigned int numLights;
		int padding;
	};

	struct PointLight
	{
		Vector3 Position;
		float   Radius;
	};

	struct LightColor
	{
		Vector3 Color;
		float Intensity;
	};

private:
	Registry<Light> m_rLightRegistry;

	WindowInformation m_WindowInformation;
	Matrix4 m_m4ViewMatrix;

	Vector4 m_f4AmbientLightAndIntensity;

	std::vector<Vector3> m_vLightWorldPositions;
	std::vector<PointLight> m_vPointLights;
	std::vector<LightColor> m_vLightColors;

	int m_iComputeShader;
	int m_iLightPositionResource;
	int m_iLightPositionBuffer;
	int m_iLightIndicesUAV;
	int m_iLightIndicesResource;
	int m_iLightColorAndIntensityResource;
	int m_iLightColorAndIntensityBuffer;
	int m_iDepthTexture;
	int m_iWindowInformationBuffer;
	int m_iInverseProjectionBuffer;
	int m_iWorldViewBuffer;
	int m_iAmbientColorBuffer;

	unsigned int m_uiNumXTiles;
	unsigned int m_uiNumYTiles;

	void OnResolutionChange(void* _pData);
	void OnViewChange(void* pData);
	void OnProjectionChange(void* pData);

public:
	LightManager();
	~LightManager();

	bool Initialize();
	bool Destroy();

	void Update();

	void SetAmbientColor(float r, float g, float b);
	void SetAmbientColor(Vector3 color);
	void SetAmbientColor(Vector4 color);
	void SetAmbientIntensity(float intensity);

	Vector3 GetAmbientColor() const;
	float GetAmbientIntensity() const;

	void RegisterLight(Light* lComponent);
	void UnregisterLight(Light* lComponent);
};

