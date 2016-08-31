//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//

#include "Assets\Bone.h"

Bone::Bone()
{

}

Bone::~Bone()
{

}

int Bone::GetParentIndex()
{
	return m_ParentIndex;
}

std::vector<int>& Bone::GetChildrenIndices()
{
	return m_ChildIndices;
}

Matrix4& Bone::GetBindPose()
{
	return m_BindPose;
}

Matrix4& Bone::GetInverseGlobalBindPose()
{
	return m_InverseGlobalBindPose;
}

Matrix4& Bone::GetBoneOffset()
{
	return m_BoneOffset;
}

void Bone::SetParentIndex(int _index)
{
	m_ParentIndex = _index;
}

void Bone::SetBindPose(Matrix4& _matrix)
{
	m_BindPose = _matrix;
}

void Bone::SetInverseGlobalBindPose(Matrix4& _matrix)
{
	m_InverseGlobalBindPose = _matrix;
}

void Bone::SetBoneOffset(Matrix4& _matrix)
{
	m_BoneOffset = _matrix;
}