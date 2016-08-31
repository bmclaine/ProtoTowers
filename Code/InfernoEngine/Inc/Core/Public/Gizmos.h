#pragma once

#include "MathLib.h"
#include "Color.h"
#include "ShaderInfo.h"
#include "Assets\Mesh.h"

class Gizmos
{
private:
	static int s_iInputLayout;
	static TopologyType s_ttTopology;
	static int s_iVertexBuffer;
	static int s_iStride;
	static int s_iIndexBuffer;
	static int s_iVertexShader;
	static int s_iGeometryShader;
	static int s_iWireEffect;
	static int s_iFillEffect;
	static Color s_cColor;

	static Mesh s_mWireCubeMesh;
	static Mesh s_mWireSphereMesh;
	static Mesh s_mWireCylinderMesh;
	static Mesh s_mHalfSphereMesh;

	static bool Initialize();

	// IN: void
	// OUT: void
	//
	// Sets up the renderer to render Gizmos
	static void StartDraw();

public:
	// IN: Color - The color to use to draw
	// OUT: void
	//
	// Sets the draw color for the next gizmo
	static void SetColor(Color color);

	// IN: Vector3    - Center Point of the Cube
	//     Vector3    - Extents of the Cube
	//     Quaternion - Rotation of the Cube
	// OUT: void
	//
	// Draws a wire cube at the specified location at the specified size
	static void DrawWireCube(Vector3 Position, Vector3 Size, Quaternion Rotation = Quaternion::Identity());
	// IN: Vector3    - Center Point of the Sphere
	//     float      - Radius of the Sphere
	//     Quaternion - Rotation of the Sphere
	// OUT: void
	//
	// Draws a wire sphere at the specified location at the specified size
	static void DrawWireSphere(Vector3 Position, float Radius, Quaternion Rotation = Quaternion::Identity());
	// IN: Vector3    - Center Point of the Capsule
	//     float      - Hieght of the Capsule
	//     float      - Radius of the Capsule
	//     Quaternion - Rotation of the Capsule
	// OUT: void
	//
	// Draws a wire capsule at the specified location at the specified size
	static void DrawWireCapsule(Vector3 Position, float Height, float Radius, Quaternion Rotation = Quaternion::Identity());
	// IN: Mesh*      - Mesh to render
	//     Vector3    - Position of the mesh
	//     Vector3    - Scale of the mesh
	//     Quaternion - Rotation of the mesh
	// OUT: void
	//
	// Draws a wired mesh at the specified location with the specified scale and rotation
	static void DrawWireMesh(Mesh* mesh, Vector3 Position, Vector3 Scale, Quaternion Rotation = Quaternion::Identity());
	// IN: Mesh*      - Mesh to render
	//     Vector3    - Position of the mesh
	//     Vector3    - Scale of the mesh
	//     Quaternion - Rotation of the mesh
	// OUT: void
	//
	// Draws a mesh at the specified location with the specified scale and rotation
	static void DrawMesh(Mesh* mesh, Vector3 Position, Vector3 Scale, Quaternion Rotation = Quaternion::Identity());

	friend class InfernoEngine;
};

