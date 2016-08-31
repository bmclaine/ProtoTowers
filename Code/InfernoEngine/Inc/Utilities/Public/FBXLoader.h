//*********************************************************************//
//	Author:		Brian McLaine
//
//	Purpose:	FBXLoader class is responsible for loading
//				mesh, texture and animation data.
//*********************************************************************//

#pragma once

#define FBXSDK_SHARED
#include <fbxsdk.h>

#include "GlobalIncludes.h"

#include "Assets\Mesh.h"
#include "Assets\Texture.h"
#include "Assets\Animation.h"

#include <unordered_map>

class FBXLoader
{
public:

	FBXLoader();
	~FBXLoader();

	//*********************************************************************//
	//	In:
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	bool LoadMeshFromFBX(std::string _filename, Mesh& _mesh);

	//*********************************************************************//
	//	In:	
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	void WriteMeshToBinary(std::string _filename, Mesh& _mesh);

	//*********************************************************************//
	//	In:	
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	void LoadMeshFromBinary(std::string _filename, Mesh& _mesh);

	//*********************************************************************//
	//	In:	
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	bool LoadAnimationFromFBX(std::string _filename, Animation& _animation);

	//*********************************************************************//
	//	In:	
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	void LoadAnimationFromBinary(std::string _filename, Animation& _animation);

	//*********************************************************************//
	//	In:		
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	void WriteAnimationToBinary(std::string _filename, Animation& _animation);

	//*********************************************************************//
	//	In:		
	//
	//	Out:	
	//
	//	Description:	
	//*********************************************************************//
	//bool LoadLevelFromFBX(std::string _filename);

private:

	bool ProcessMeshNode(FbxNode* _sceneNode, Mesh& _mesh, std::vector<FbxNode*>& _fbxBones);
	//bool ProcessAnimMeshNode(FbxNode* _sceneNode, std::vector<FbxNode*>& _fbxBones);

	bool ProcessMeshSkeletonNode(FbxNode* _clusterNode, std::vector<Bone>& _bones, std::vector<FbxNode*>& _fbxBones, int& index);
	//bool ProcessAnimSkeletonNode(FbxNode* _clusterNode, std::vector<FbxNode*>& _fbxBodes);

	//bool ProcessLevelMeshNode(FbxNode* _sceneNode, std::unordered_map<std::string, std::vector<Matrix4>>& _levelData);

	bool LoadMeshData(FbxMesh* _fbxMesh, std::vector<LoadedVertex>& _verticies, std::vector<unsigned int>& _indices);
	bool LoadMeshSkinData(FbxMesh* _fbxMesh, FbxAMatrix& _meshWorld, std::vector<LoadedVertex>& _vertices, std::vector<Bone>& _bones, std::vector<std::vector<BoneInfluence>>& _influences, std::vector<FbxNode*> _fbxBones);
	void CreateBoundingSphere(Mesh& _mesh);

	bool LoadAnimation(FbxAnimLayer* _animLayer, FbxNode* _node, Animation& _animation/*, std::vector<FbxNode*>& fbxBones*/);
	bool LoadAnimation(FbxAnimLayer* _animLayer, FbxNode* _node, FbxAMatrix& _meshWorld, Animation& _animation);
	void LoadAnimationData(FbxAnimLayer* _animLayer, FbxMesh* _mesh, FbxAMatrix& _meshWorld, Animation& _animation);
	//bool LoadAnimSkinData(FbxMesh* _fbxMesh, std::vector<FbxNode*>& _fbxNodes);

	void LoadCurve(FbxAnimCurve* animCurve, std::vector<float>& keyTimes);

	//void ExportLevelData(std::unordered_map<std::string, std::vector<Matrix4>>& _levelData);

	Matrix4 ConvertFBXMatrix(const FbxAMatrix& _fbxMatrix);

	void GetBoneInfo(FbxMesh* _mesh, FbxCluster* _cluster, FbxAMatrix& _meshWorld, FbxTime _time, FbxAMatrix& _boneWorld, FbxAMatrix& _boneOffset);

	FbxAMatrix GetLocalTransform(FbxNode* _meshNode, FbxTime _time);
	FbxAMatrix GetGlobalTransform(FbxNode* _meshNode, FbxTime _time/*, FbxCluster* _cluster*/);
	FbxAMatrix GetGeometryTranslation(FbxNode* _meshNode);

	void PadVerticeBoneInfluences(std::vector<LoadedVertex>& _verts, std::vector<std::vector<BoneInfluence>>& _influences);
	void RightToLeftHanded(std::vector<unsigned int>& _indices);
};

