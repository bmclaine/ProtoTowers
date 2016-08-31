//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Bone represents one joint of a Skeleton Hierarchy
//*********************************************************************//
#pragma once

#include "GlobalIncludes.h"
#include "MathLib.h"

class Bone
{
public:

	Bone();
	~Bone();

	int GetParentIndex();
	std::vector<int>& GetChildrenIndices();

	Matrix4& GetBindPose();
	Matrix4& GetInverseGlobalBindPose();
	Matrix4& GetBoneOffset();

	void SetParentIndex(int _index);

	void SetBindPose(Matrix4& _matrix);
	void SetInverseGlobalBindPose(Matrix4& _matrix);
	void SetBoneOffset(Matrix4& _matrix);

	std::string m_Name;

private:

	int m_ParentIndex;
	std::vector<int> m_ChildIndices;

	Matrix4 m_BindPose;
	Matrix4 m_InverseGlobalBindPose;
	Matrix4 m_BoneOffset;
};