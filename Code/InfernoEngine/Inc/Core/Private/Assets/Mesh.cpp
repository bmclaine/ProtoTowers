//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Mesh class is the object responsible for
//					storing vertex and index data
//*********************************************************************//

#include "Assets/Mesh.h"

//*********************************************************************//
//	In:				rhs - A referece to the Vertex we're comparing
//					against
//	Out:			true - if this Vertex is less than rhs
//					false - if this Vertex is greater than rhs
//	Description:	Gets the Mesh's List of Indicies
//*********************************************************************//

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

std::vector<SubMesh>& Mesh::GetSubMeshes()
{
	return m_SubMeshes;
}

std::vector<Bone>& Mesh::GetBones()
{
	return m_Bones;
}

Sphere& Mesh::GetBoundingSphere()
{
	return m_spBoundingSphere;
}

void Mesh::SetBoundingSpehere(Sphere& _sphere)
{
	m_spBoundingSphere = _sphere;
}

//*********************************************************************//
//	In:				Void
//	Out:			Void
//
//	Description:	Called after verts and indices have been loaded into
//					the graphics manager. Sets indicie count and clears
//					up mememory stored in vertex/index vectors.
//*********************************************************************//
void Mesh::Finalize()
{
	size_t subMeshCount = m_SubMeshes.size();
	for (size_t subMesh = 0; subMesh < subMeshCount; subMesh++)
	{
		m_SubMeshes[subMesh].indiceCount = (unsigned int)m_SubMeshes[subMesh].indices.size();
		m_SubMeshes[subMesh].vertexCount = (unsigned int)m_SubMeshes[subMesh].vertices.size();

		m_SubMeshes[subMesh].vertices.clear();
		m_SubMeshes[subMesh].indices.clear();
	}
}
