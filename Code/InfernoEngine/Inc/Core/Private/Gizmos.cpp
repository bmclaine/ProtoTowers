#include "Gizmos.h"
#include "Assets\Mesh.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "../../Interface/Public/Engine.h"

int Gizmos::s_iInputLayout = -1;
TopologyType Gizmos::s_ttTopology = TopologyType::LINE_TOPOLOGY;
int Gizmos::s_iVertexBuffer = -1;
int Gizmos::s_iStride = 0;
int Gizmos::s_iIndexBuffer = -1;
int Gizmos::s_iVertexShader = -1;
int Gizmos::s_iGeometryShader = -1;
int Gizmos::s_iFillEffect = -1;
int Gizmos::s_iWireEffect = -1;
Color Gizmos::s_cColor;

Mesh Gizmos::s_mWireCubeMesh;
Mesh Gizmos::s_mWireSphereMesh;
Mesh Gizmos::s_mWireCylinderMesh;
Mesh Gizmos::s_mHalfSphereMesh;

bool Gizmos::Initialize()
{
	s_iInputLayout = InfernoEngine_Graphics::GetInputLayout(L"LineLayout");
	s_iVertexBuffer = InfernoEngine_Graphics::GetBuffer(VERTEX_BUFFER, L"VertexBuffer");
	s_iStride = sizeof(Vertex);
	s_iIndexBuffer = InfernoEngine_Graphics::GetBuffer(INDEX_BUFFER, L"IndexBuffer");
	s_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"LineVertexShader");
	s_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"GizmoMeshGeometryShader");
	s_cColor = Color(0.0f, 1.0f, 0.0f, 1.0f);

	int pixelShader = InfernoEngine_Graphics::GetShader(PIXEL_SHADER, L"ColorOnlyPixelShader");
	int samplerState = -1;
	s_iWireEffect = Inferno::CreateEffect("GizmosWiredEffect", pixelShader,
		InfernoEngine_Graphics::GetRasterizerState(L"WireFrameRasterizerState"), InfernoEngine_Graphics::GetBlendState(L"NoBlendingBlendState"), &samplerState, 1);
	s_iFillEffect = Inferno::CreateEffect("GizmosFillEffect", pixelShader,
		InfernoEngine_Graphics::GetRasterizerState(L"FillRasterizerState"), InfernoEngine_Graphics::GetBlendState(L"AlphaBlendingBlendState"), &samplerState, 1);

	// Set up the Cube Wire Mesh
	SubMesh CubeSubMesh;

	Vertex cubeVerts[8];
	cubeVerts[0].position = Vector4(-0.5f,  0.5f,  0.5f, 1.0f);
	cubeVerts[1].position = Vector4( 0.5f,  0.5f,  0.5f, 1.0f);
	cubeVerts[2].position = Vector4(-0.5f, -0.5f,  0.5f, 1.0f);
	cubeVerts[3].position = Vector4( 0.5f, -0.5f,  0.5f, 1.0f);
	cubeVerts[4].position = Vector4(-0.5f,  0.5f, -0.5f, 1.0f);
	cubeVerts[5].position = Vector4( 0.5f,  0.5f, -0.5f, 1.0f);
	cubeVerts[6].position = Vector4(-0.5f, -0.5f, -0.5f, 1.0f);
	cubeVerts[7].position = Vector4( 0.5f, -0.5f, -0.5f, 1.0f);

	for (int i = 0; i < 8; i++)
		CubeSubMesh.vertices.push_back(cubeVerts[i]);

	unsigned int indices[24] = {
		0, 1, 0, 2, 3, 1, 3, 2,		// Back Side
		4, 5, 4, 6, 7, 5, 7, 6,		// Front Side
		4, 0, 6, 2, 5, 1, 7, 3,		// Remainder Sides
	};

	for (int i = 0; i < 24; i++)
		CubeSubMesh.indices.push_back(indices[i]);

	s_mWireCubeMesh.GetSubMeshes().push_back(CubeSubMesh);

	InfernoEngine_Graphics::LoadMesh(&s_mWireCubeMesh);
	s_mWireCubeMesh.Finalize();

	// Set up the Sphere Wire Mesh
	SubMesh SphereSubMesh;

	Vertex sphereVerts[1080];
	unsigned int sphereIndices[2160];

	// Create a sphere with 360 points on 3 different planes
	for (int i = 0; i < 360; i++)
	{
		float sinR = sin(ToRadians(i + 0.0f));
		float cosR = cos(ToRadians(i + 0.0f));

		// Gets the circle on X,Y plane
		sphereVerts[i].position = Vector4(cosR, sinR, 0.0f, 1.0f);
		// Gets the circle on Y,Z plane
		sphereVerts[360 + i].position = Vector4(0.0f, sinR, cosR, 1.0f);
		// Gets the circle on X,Z plane
		sphereVerts[720 + i].position = Vector4(cosR, 0.0f, sinR, 1.0f);

		// Set up the indices
		if (i < 359)
		{
			// X,Y Plane indices
			sphereIndices[i * 2] = i;
			sphereIndices[(i * 2) + 1] = i + 1;

			// Y,Z Plane indices
			sphereIndices[(i * 2) + 720] = i + 360;
			sphereIndices[(i * 2) + 721] = i + 361;

			// X,Z Plane indices
			sphereIndices[(i * 2) + 1440] = i + 720;
			sphereIndices[(i * 2) + 1441] = i + 721;
		}
		else
		{
			// X,Y Plane indices
			sphereIndices[i * 2] = i;
			sphereIndices[(i * 2) + 1] = 0;

			// Y,Z Plane indices
			sphereIndices[(i * 2) + 720] = i + 360;
			sphereIndices[(i * 2) + 721] = 360;

			// X,Z Plane indices
			sphereIndices[(i * 2) + 1440] = i + 720;
			sphereIndices[(i * 2) + 1441] = 720;
		}
	}

	SphereSubMesh.vertices.reserve(1080);
	for (int i = 0; i < 1080; i++)
		SphereSubMesh.vertices.push_back(sphereVerts[i]);

	SphereSubMesh.indices.reserve(2160);
	for (int i = 0; i < 2160; i++)
		SphereSubMesh.indices.push_back(sphereIndices[i]);

	s_mWireSphereMesh.GetSubMeshes().push_back(SphereSubMesh);

	InfernoEngine_Graphics::LoadMesh(&s_mWireSphereMesh);
	s_mWireSphereMesh.Finalize();

	// Set up Capsule Wire Mesh
	SubMesh CylinderSubMesh;

	Vertex cylinderVerts[720];
	unsigned int cylinderIndices[1448];

	// Set up the circles for the top and bottom of the cylinder part
	for (int i = 0; i < 360; i++)
	{
		// Top half circle
		cylinderVerts[i].position = sphereVerts[i + 720].position;
		cylinderVerts[i].position.y += 0.5f;
		
		// Bottom half circle
		cylinderVerts[i + 360].position = sphereVerts[i + 720].position;
		cylinderVerts[i + 360].position.y -= 0.5f;

		if (i < 359)
		{
			// Top half circle
			cylinderIndices[i * 2] = i;
			cylinderIndices[(i * 2) + 1] = i + 1;
			
			// Bottom half circle
			cylinderIndices[(i * 2) + 720] = i + 360;
			cylinderIndices[(i * 2) + 721] = i + 361;
		}
		else
		{
			// Top half circle
			cylinderIndices[i * 2] = i;
			cylinderIndices[(i * 2) + 1] = 0;

			// Bottom half circle
			cylinderIndices[(i * 2) + 720] = i + 360;
			cylinderIndices[(i * 2) + 721] = 360;
		}
	}

	cylinderIndices[1440] = 0;
	cylinderIndices[1441] = 360;
	cylinderIndices[1442] = 90;
	cylinderIndices[1443] = 450;
	cylinderIndices[1444] = 180;
	cylinderIndices[1445] = 540;
	cylinderIndices[1446] = 270;
	cylinderIndices[1447] = 630;

	CylinderSubMesh.vertices.reserve(720);
	for (int i = 0; i < 720; i++)
		CylinderSubMesh.vertices.push_back(cylinderVerts[i]);

	CylinderSubMesh.indices.reserve(1448);
	for (int i = 0; i < 1448; i++)
		CylinderSubMesh.indices.push_back(cylinderIndices[i]);

	s_mWireCylinderMesh.GetSubMeshes().push_back(CylinderSubMesh);
	InfernoEngine_Graphics::LoadMesh(&s_mWireCylinderMesh);
	s_mWireCylinderMesh.Finalize();

	SubMesh HalfCircleSubMesh;

	Vertex halfSphereVerts[360];
	unsigned int halfSphereIndices[716];

	// Set up the half circles
	for (int i = 0; i < 180; i++)
	{
		halfSphereVerts[i].position = sphereVerts[i].position;

		halfSphereVerts[i + 180].position = sphereVerts[i + 360].position;

		if (i < 179)
		{
			halfSphereIndices[(i * 2)] = i;
			halfSphereIndices[(i * 2) + 1] = i + 1;

			halfSphereIndices[(i * 2) + 358] = i + 180;
			halfSphereIndices[(i * 2) + 359] = i + 181;
		}
	}

	HalfCircleSubMesh.vertices.reserve(360);
	for (int i = 0; i < 360; i++)
		HalfCircleSubMesh.vertices.push_back(halfSphereVerts[i]);

	HalfCircleSubMesh.indices.reserve(716);
	for (int i = 0; i < 716; i++)
		HalfCircleSubMesh.indices.push_back(halfSphereIndices[i]);

	s_mHalfSphereMesh.GetSubMeshes().push_back(HalfCircleSubMesh);
	InfernoEngine_Graphics::LoadMesh(&s_mHalfSphereMesh);
	s_mHalfSphereMesh.Finalize();

	return true;
}

// IN: void
// OUT: void
//
// Sets up the renderer to render Gizmos
void Gizmos::StartDraw()
{
	InfernoEngine_Graphics::SetInputLayout(s_iInputLayout);
	InfernoEngine_Graphics::SetTopology(s_ttTopology);
	InfernoEngine_Graphics::SetVertexBuffer(s_iVertexBuffer, s_iStride, 0);
	InfernoEngine_Graphics::SetIndexBuffer(s_iIndexBuffer);
	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, s_iVertexShader);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);		// Set to no geometry by default, as most draws will be the custom wire objects
	InfernoEngine_Graphics::SetColor(s_cColor);
}

// IN: Color - The color to use to draw
// OUT: void
//
// Sets the draw color for the next gizmo
void Gizmos::SetColor(Color color)
{
	InfernoEngine_Graphics::SetColor(color);
}

// IN: Vector3    - Center Point of the Cube
//     Vector3    - Extents of the Cube
//     Quaternion - Rotation of the Cube
// OUT: void
//
// Draws a wire cube at the specified location at the specified size
void Gizmos::DrawWireCube(Vector3 Position, Vector3 Size, Quaternion Rotation)
{
	Matrix4 world = Matrix4::Scale(Size) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	InfernoEngine_Graphics::SetWorldMatrix(world);
	InfernoEngine_Graphics::Draw(s_iWireEffect, &s_mWireCubeMesh);
}
// IN: Vector3    - Center Point of the Sphere
//     float      - Radius of the Sphere
//     Quaternion - Rotation of the Sphere
// OUT: void
//
// Draws a wire sphere at the specified location at the specified size
void Gizmos::DrawWireSphere(Vector3 Position, float Radius, Quaternion Rotation)
{
	Matrix4 world = Matrix4::Scale(Vector3(Radius, Radius, Radius)) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	InfernoEngine_Graphics::SetWorldMatrix(world);
	InfernoEngine_Graphics::Draw(s_iWireEffect, &s_mWireSphereMesh);
}
// IN: Vector3    - Center Point of the Capsule
//     float      - Hieght of the Capsule
//     float      - Radius of the Capsule
//     Quaternion - Rotation of the Capsule
// OUT: void
//
// Draws a wire capsule at the specified location at the specified size
void Gizmos::DrawWireCapsule(Vector3 Position, float Height, float Radius, Quaternion Rotation)
{
	Vector3 newPosition = Position;
	newPosition.y += Height * 0.5f;
	Matrix4 halfSphereWorld = Matrix4::Scale(Vector3(Radius, Radius, Radius)) * Rotation.ToMatrix4() * Matrix4::Translate(newPosition);
	Matrix4 cylinderWorld = Matrix4::Scale(Vector3(Radius, Height, Radius)) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	InfernoEngine_Graphics::SetWorldMatrix(cylinderWorld);
	InfernoEngine_Graphics::Draw(s_iWireEffect, &s_mWireCylinderMesh);

	InfernoEngine_Graphics::SetWorldMatrix(halfSphereWorld);
	InfernoEngine_Graphics::Draw(s_iWireEffect, &s_mHalfSphereMesh);

	newPosition.y -= Height;
	halfSphereWorld = Matrix4::Scale(Vector3(Radius, Radius, Radius)) * Matrix4::RotationX(ToRadians(180)) * Rotation.ToMatrix4() * Matrix4::Translate(newPosition);
	InfernoEngine_Graphics::SetWorldMatrix(halfSphereWorld);
	InfernoEngine_Graphics::Draw(s_iWireEffect, &s_mHalfSphereMesh);
}
// IN: Mesh*      - Mesh to render
//     Vector3    - Position of the mesh
//     Vector3    - Scale of the mesh
//     Quaternion - Rotation of the mesh
// OUT: void
//
// Draws a wired mesh at the specified location with the specified scale and rotation
void Gizmos::DrawWireMesh(Mesh* mesh, Vector3 Position, Vector3 Scale, Quaternion Rotation)
{
	Matrix4 world = Matrix4::Scale(Scale) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	InfernoEngine_Graphics::SetWorldMatrix(world);
	InfernoEngine_Graphics::SetTopology(TopologyType::TRIANGLE_TOPOLOGY);
	InfernoEngine_Graphics::Draw(s_iWireEffect, mesh);
	InfernoEngine_Graphics::SetTopology(s_ttTopology);
}
// IN: Mesh*      - Mesh to render
//     Vector3    - Position of the mesh
//     Vector3    - Scale of the mesh
//     Quaternion - Rotation of the mesh
// OUT: void
//
// Draws a mesh at the specified location with the specified scale and rotation
void Gizmos::DrawMesh(Mesh* mesh, Vector3 Position, Vector3 Scale, Quaternion Rotation)
{
	Matrix4 world = Matrix4::Scale(Scale) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	InfernoEngine_Graphics::SetWorldMatrix(world);
	InfernoEngine_Graphics::SetTopology(TopologyType::TRIANGLE_TOPOLOGY);
	InfernoEngine_Graphics::Draw(s_iFillEffect, mesh);
	InfernoEngine_Graphics::SetTopology(s_ttTopology);
}