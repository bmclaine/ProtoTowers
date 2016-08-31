//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Mesh class is the object responsible for
//					storing vertex, index and uv data.
//*********************************************************************//

#pragma once

#include "GlobalIncludes.h"
#include "MathLib.h"
#include "Vertex.h"
#include "Bone.h"
#include "Physics\Collision.h"

struct SubMesh
{
	std::string name;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int indiceCount;
	unsigned int startIndex;
	unsigned int startVertex;
	unsigned int vertexCount;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	std::vector<SubMesh>& GetSubMeshes();
	std::vector<Bone>& GetBones();
	Sphere& GetBoundingSphere();
	void SetBoundingSpehere(Sphere& _sphere);

	//*********************************************************************//
	//	In:				Void
	//	Out:			Void
	//
	//	Description:	Clears up vertex and index vectors after they've
	//					been loaded into the graphics manager.
	//*********************************************************************//
	void Finalize();

	std::string m_Name;

	

private:
	std::vector<SubMesh> m_SubMeshes;
	std::vector<Bone> m_Bones;

	Sphere m_spBoundingSphere;
};

