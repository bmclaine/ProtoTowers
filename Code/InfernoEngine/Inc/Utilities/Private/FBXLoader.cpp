//*********************************************************************//
//	Author:		Brian McLaine
//
//	Purpose:	FBXLoader loads Mesh, Texture and Animation Data
//				from either FBX or binary depending on which files
//				exist and which one is newer.
//*********************************************************************//

#include "FBXLoader.h"
#include "AssertHelper.h"
#include "Assets\Vertex.h"

#include <fstream>
#include <unordered_map>

FBXLoader::FBXLoader()
{

}

FBXLoader::~FBXLoader()
{

}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
bool FBXLoader::LoadMeshFromFBX(std::string _filename, Mesh& _mesh)
{
	FbxManager* fbxManager = FbxManager::Create();
	if (fbxManager == NULL) return false;

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	if (fbxIOSettings == NULL) return false;

	fbxManager->SetIOSettings(fbxIOSettings);

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	if (!fbxImporter) return false;

	FbxStatus status = fbxImporter->GetStatus();

	if (!ASSERT(fbxImporter->Initialize(_filename.c_str(), -1, fbxIOSettings), (std::string("FbxImporter::Initialize failed: ")
		+ fbxImporter->GetStatus().GetErrorString()).c_str()))
		return false;

	status = fbxImporter->GetStatus();

	FbxScene* fbxScene = FbxScene::Create(fbxManager, "myScene");
	if (fbxScene == NULL) return false;

	if (fbxImporter->Import(fbxScene) == false)
	{
		status = fbxImporter->GetStatus();
		return false;
	}

	fbxImporter->Destroy();

	FbxGeometryConverter fbxGeometryConverter(fbxManager);
	if (fbxGeometryConverter.Triangulate(fbxScene, true, false) == false)
		return false;

	if (fbxGeometryConverter.SplitMeshesPerMaterial(fbxScene, true) == false)
		return false;

	std::vector<FbxNode*> fbxJoints;

	FbxNode* rootNode = fbxScene->GetRootNode();
	if (ProcessMeshNode(rootNode, _mesh, fbxJoints) == false)
		return false;

	CreateBoundingSphere(_mesh);

	return true;
}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
void FBXLoader::LoadMeshFromBinary(std::string _filename, Mesh& _mesh)
{
	std::ifstream binaryRead;

	binaryRead.open(_filename.c_str(), std::ios_base::in | std::ios_base::binary);

	if (binaryRead.is_open() == true)
	{
		// Read Mesh Name Length
		unsigned int length;
		binaryRead.read((char*)&length, sizeof(unsigned int));

		// Read Mesh Name
		char* tempStr = new char[length + 1];
		binaryRead.read((char*)&tempStr[0], length);
		tempStr[length] = '\0';
		_mesh.m_Name = tempStr;
		delete tempStr;

		// Our list of SubMeshes To Fill
		std::vector<SubMesh> subMeshes;

		// Read SubMesh Count
		unsigned int subMeshCount = 0;
		binaryRead.read((char*)&subMeshCount, sizeof(unsigned int));
		subMeshes.reserve(subMeshCount);

		// For Each SubMesh..
		for (unsigned int subMesh = 0; subMesh < subMeshCount; subMesh++)
		{
			SubMesh newSubMesh;

			// Read Mesh Name Length
			unsigned int length;
			binaryRead.read((char*)&length, sizeof(unsigned int));

			// Read Mesh Name
			char* tempStr = new char[length + 1];
			binaryRead.read((char*)&tempStr[0], length);
			tempStr[length] = '\0';
			newSubMesh.name = tempStr;
			delete tempStr;

			// Current Vert List
			std::vector<Vertex> verts;

			// Read Vert Count
			unsigned int vertCount = 0;
			binaryRead.read((char*)&vertCount, sizeof(unsigned int));
			verts.reserve(vertCount);

			// For Each Vert..
			for (unsigned int vert = 0; vert < vertCount; vert++)
			{
				Vertex newVert;

				// Read Position, Normal, UV, TBN and ControlPointIndex
				binaryRead.read((char*)&newVert.position, sizeof(Vector4));
				binaryRead.read((char*)&newVert.normal, sizeof(Vector3));
				binaryRead.read((char*)&newVert.tangent, sizeof(Vector3));
				binaryRead.read((char*)&newVert.uv, sizeof(Vector2));

				Vector4 boneIndices, boneWeights;
				float index, weight;

				// Read Bone Index 1
				binaryRead.read((char*)&index, sizeof(float));
				boneIndices.x = index;

				// Read Bone Index 2
				binaryRead.read((char*)&index, sizeof(float));
				boneIndices.y = index;

				// Read Bone Index 3
				binaryRead.read((char*)&index, sizeof(float));
				boneIndices.z = index;

				// Read Bone Index 4
				binaryRead.read((char*)&index, sizeof(float));
				boneIndices.w = index;

				// Read Bone Weight 1
				binaryRead.read((char*)&weight, sizeof(float));
				boneWeights.x = weight;

				// Read Bone Weight 2
				binaryRead.read((char*)&weight, sizeof(float));
				boneWeights.y = weight;

				// Read Bone Weight 3
				binaryRead.read((char*)&weight, sizeof(float));
				boneWeights.z = weight;

				// Read Bone Weight 4
				binaryRead.read((char*)&weight, sizeof(float));
				boneWeights.w = weight;

				newVert.boneIndices = boneIndices;
				newVert.boneWeights = boneWeights;

				// Add This Vert To The Vert List
				verts.push_back(newVert);
			}

			// Set This SubMeshes Vertices
			newSubMesh.vertices = verts;

			// Current Indicie List
			std::vector<unsigned int> indices;

			// Read Indicie Count
			unsigned int indiceCount = 0;
			binaryRead.read((char*)&indiceCount, sizeof(unsigned int));
			indices.reserve(indiceCount);

			// For Each Indice..
			for (unsigned int indice = 0; indice < indiceCount; indice++)
			{
				// Read Index
				unsigned int index = 0;
				binaryRead.read((char*)&index, sizeof(unsigned int));
				indices.push_back(index);
			}

			// Set This SubMeshes Indices
			newSubMesh.indices = indices;

			// Add The SubMesh To This Mesh's SubMesh List
			_mesh.GetSubMeshes().push_back(newSubMesh);
		}

		// Bone List To Fill
		std::vector<Bone> bones;

		// Read Bone Count
		unsigned int boneCount = 0;
		binaryRead.read((char*)&boneCount, sizeof(unsigned int));
		bones.reserve(boneCount);

		// For Each Bone..
		for (unsigned int bone = 0; bone < boneCount; bone++)
		{
			Bone newBone;

			// Read The Length Of The Bone Name
			unsigned int length = 0;
			binaryRead.read((char*)&length, sizeof(unsigned int));

			// Read The Bone Name
			char* tempStr = new char[length + 1];
			binaryRead.read((char*)&tempStr[0], length);
			tempStr[length] = '\0';
			newBone.m_Name = tempStr;
			delete tempStr;

			// Read Parent Bone Index
			unsigned int parentIndex = 0;
			binaryRead.read((char*)&parentIndex, sizeof(unsigned int));
			newBone.SetParentIndex(parentIndex);

			// Read Child Count
			unsigned int childCount = 0;
			binaryRead.read((char*)&childCount, sizeof(unsigned int));
			newBone.GetChildrenIndices().reserve(childCount);

			// List Of Children To Read
			std::vector<int> children;
			children.reserve(childCount);

			// For Each Child..
			for (unsigned int child = 0; child < childCount; child++)
			{
				// Read Child Bone Index
				unsigned int index = 0;
				binaryRead.read((char*)&index, sizeof(unsigned int));

				// Add The Index
				children.push_back(index);
			}

			// Set This Bones Children
			newBone.GetChildrenIndices() = children;

			// Read Global Inverse Bind Pose Matrix
			Matrix4 globalBindPose;
			binaryRead.read((char*)&globalBindPose, sizeof(Matrix4));
			newBone.SetBindPose(globalBindPose);

			// Read Global Inverse Bind Pose Matrix
			Matrix4 inverseGlobalBindPose;
			binaryRead.read((char*)&inverseGlobalBindPose, sizeof(Matrix4));
			newBone.SetInverseGlobalBindPose(inverseGlobalBindPose);

			// Read BoneOffset Matrix
			Matrix4 boneOffset;
			binaryRead.read((char*)&boneOffset, sizeof(Matrix4));
			newBone.SetBoneOffset(boneOffset);

			// Add The Bone
			bones.push_back(newBone);
		}

		// Set This Mesh's Bones
		_mesh.GetBones() = bones;

		// Read Bounding Sphere
		Sphere boundingSphere;
		// Read Center
		binaryRead.read((char*)&boundingSphere.center, sizeof(Vector3));
		// Read Radius
		binaryRead.read((char*)&boundingSphere.radius, sizeof(float));
		// Set Sphere
		_mesh.SetBoundingSpehere(boundingSphere);

		binaryRead.close();
	}
}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
void FBXLoader::WriteMeshToBinary(std::string _fileName, Mesh& _mesh)
{
	std::ofstream binaryWrite;

	binaryWrite.open(_fileName.c_str(), std::ios_base::out | std::ios_base::binary);

	if (binaryWrite.is_open() == true)
	{
		// Write Mesh Name Length
		const char* str = _mesh.m_Name.c_str();
		unsigned int length = (unsigned int)strlen(str);
		binaryWrite.write((char*)&length, sizeof(unsigned int));

		// Write Mesh Name
		binaryWrite.write(str, length);

		// Get copy of subMesh list
		std::vector<SubMesh> subMeshes = _mesh.GetSubMeshes();

		// Write SubMesh Count
		unsigned int subMeshCount = (unsigned int)subMeshes.size();
		binaryWrite.write((char*)&subMeshCount, sizeof(unsigned int));

		// For Each SubMesh..
		for (unsigned int subMesh = 0; subMesh < subMeshCount; subMesh++)
		{
			// Write SubMesh Name Length
			const char* str = subMeshes[subMesh].name.c_str();
			unsigned int length = (unsigned int)strlen(str);
			binaryWrite.write((char*)&length, sizeof(unsigned int));

			// Write SubMesh Name
			binaryWrite.write(str, length);

			// Grab Copy Of This SubMesh's Vert List
			std::vector<Vertex> verts = subMeshes[subMesh].vertices;

			// Write Vert Count
			unsigned int vertCount = (unsigned int)verts.size();
			binaryWrite.write((char*)&vertCount, sizeof(unsigned int));

			// For Each Vertex
			for (unsigned int vert = 0; vert < vertCount; vert++)
			{
				// Write Position, Normal, UV, TBN and ControlPointIndex
				binaryWrite.write((char*)&verts[vert].position, sizeof(Vector4));
				binaryWrite.write((char*)&verts[vert].normal, sizeof(Vector3));
				binaryWrite.write((char*)&verts[vert].tangent, sizeof(Vector3));
				binaryWrite.write((char*)&verts[vert].uv, sizeof(Vector2));

				// Write Bone Index 1
				float index = verts[vert].boneIndices.x;
				binaryWrite.write((char*)&index, sizeof(float));

				// Write Bone Index 2
				index = verts[vert].boneIndices.y;
				binaryWrite.write((char*)&index, sizeof(float));

				// Write Bone Index 3
				index = verts[vert].boneIndices.z;
				binaryWrite.write((char*)&index, sizeof(float));

				// Write Bone Index 4
				index = verts[vert].boneIndices.w;
				binaryWrite.write((char*)&index, sizeof(float));

				// Write Bone Weight 1
				float weight = verts[vert].boneWeights.x;
				binaryWrite.write((char*)&weight, sizeof(float));

				// Write Bone Weight 2
				weight = verts[vert].boneWeights.y;
				binaryWrite.write((char*)&weight, sizeof(float));

				// Write Bone Weight 3
				weight = verts[vert].boneWeights.z;
				binaryWrite.write((char*)&weight, sizeof(float));

				// Write Bone Weight 4
				weight = verts[vert].boneWeights.w;
				binaryWrite.write((char*)&weight, sizeof(float));
			}

			// Grab Copy Of This SubMesh's Indices
			std::vector<unsigned int> indices = subMeshes[subMesh].indices;

			// Write Indicie Count
			unsigned int indiceCount = (unsigned int)indices.size();
			binaryWrite.write((char*)&indiceCount, sizeof(unsigned int));

			// For Each Indice..
			for (unsigned int indice = 0; indice < indiceCount; indice++)
			{
				// Write Index
				binaryWrite.write((char*)&indices[indice], sizeof(unsigned int));
			}
		}

		// Grab Copy Of The Mesh's Bones
		std::vector<Bone> bones = _mesh.GetBones();

		// Write Bone Count
		unsigned int boneCount = (unsigned int)bones.size();
		binaryWrite.write((char*)&boneCount, sizeof(unsigned int));

		// For Each Bone..
		for (unsigned int bone = 0; bone < boneCount; bone++)
		{
			// Write The Length Of The Bone Name
			const char* str = bones[bone].m_Name.c_str();
			unsigned int length = (unsigned int)strlen(str);
			binaryWrite.write((char*)&length, sizeof(unsigned int));

			// Write The Bone Name
			binaryWrite.write(str, length);

			// Write Parent Bone Index
			unsigned int parentIndex = bones[bone].GetParentIndex();
			binaryWrite.write((char*)&parentIndex, sizeof(unsigned int));

			// Write Child Count
			unsigned int childCount = (unsigned int)bones[bone].GetChildrenIndices().size();
			binaryWrite.write((char*)&childCount, sizeof(unsigned int));

			// For Each Child..
			for (unsigned int child = 0; child < childCount; child++)
			{
				// Write Child Bone Index
				unsigned int index = bones[bone].GetChildrenIndices()[child];
				binaryWrite.write((char*)&index, sizeof(unsigned int));
			}

			// Write Global Bind Pose Matrix
			Matrix4 globalBindPose = bones[bone].GetBindPose();
			binaryWrite.write((char*)&globalBindPose, sizeof(Matrix4));

			// Write Global Inverse Bind Pose Matrix
			Matrix4 inverseGlobalBindPose = bones[bone].GetInverseGlobalBindPose();
			binaryWrite.write((char*)&inverseGlobalBindPose, sizeof(Matrix4));

			// Write BoneOffset Matrix
			Matrix4 boneOffset = bones[bone].GetBoneOffset();
			binaryWrite.write((char*)&boneOffset, sizeof(Matrix4));
		}

		// Write BoundingSphere
		Sphere boundingSphere = _mesh.GetBoundingSphere();
		// Sphere center
		binaryWrite.write((char*)&boundingSphere.center, sizeof(Vector3));
		// Sphere radius
		binaryWrite.write((char*)&boundingSphere.radius, sizeof(float));

		// // Write Texture Name Count
		// unsigned int textureNameCount = (unsigned int)_textures.size();
		// binaryWrite.write((char*)&textureNameCount, sizeof(unsigned int));
		// 
		// // For Each Texture Name
		// for (unsigned int textureName = 0; textureName < textureNameCount; textureName++)
		// {
		// 	// Write The Length Of The Texture Name
		// 	const char* str = _textures[textureName].c_str();
		// 	unsigned int length = (unsigned int)strlen(str);
		// 	binaryWrite.write((char*)&length, sizeof(unsigned int));
		// 
		// 	// Write The Texture Name
		// 	binaryWrite.write(str, length);
		// }
	}
}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
bool FBXLoader::LoadAnimationFromFBX(std::string _filename, Animation& _animation)
{
	FbxManager* fbxManager = FbxManager::Create();
	if (fbxManager == NULL) return false;

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	if (fbxIOSettings == NULL) return false;

	fbxManager->SetIOSettings(fbxIOSettings);

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	if (!fbxImporter) return false;

	FbxStatus status = fbxImporter->GetStatus();

	if (!ASSERT(fbxImporter->Initialize(_filename.c_str(), -1, fbxIOSettings), (std::string("FbxImporter::Initialize failed: ")
		+ fbxImporter->GetStatus().GetErrorString()).c_str()))
		return false;

	status = fbxImporter->GetStatus();

	FbxScene* fbxScene = FbxScene::Create(fbxManager, "myScene");
	if (fbxScene == NULL) return false;

	if (fbxImporter->Import(fbxScene) == false)
	{
		status = fbxImporter->GetStatus();
		return false;
	}

	fbxImporter->Destroy();

	FbxGeometryConverter fbxGeometryConverter(fbxManager);
	if (fbxGeometryConverter.Triangulate(fbxScene, true, false) == false)
		return false;

	if (fbxGeometryConverter.SplitMeshesPerMaterial(fbxScene, true) == false)
		return false;

	//std::vector<FbxNode*> fbxJoints;

	FbxNode* rootNode = fbxScene->GetRootNode();
	//if (ProcessAnimMeshNode(rootNode, fbxJoints) == false)
		//return false;

	unsigned int numAnimStacks = fbxScene->GetSrcObjectCount<FbxAnimStack>();

	FbxAnimStack* animStack;

	for (unsigned int animStackIndex = 0; animStackIndex < numAnimStacks; animStackIndex++)
	{
		animStack = fbxScene->GetSrcObject<FbxAnimStack>(animStackIndex);

		_animation.SetDuration((float)(animStack->GetReferenceTimeSpan().GetStop() - animStack->GetReferenceTimeSpan().GetStart()).GetSecondDouble());

		int numAnimLayers = animStack->GetMemberCount<FbxAnimLayer>();

		FbxAnimLayer* animLayer;

		for (int animLayerIndex = 0; animLayerIndex < numAnimLayers; animLayerIndex++)
		{
			animLayer = animStack->GetMember<FbxAnimLayer>(animLayerIndex);

			LoadAnimation(animLayer, rootNode, _animation/*, fbxJoints*/);
		}
	}

	return true;
}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
void FBXLoader::LoadAnimationFromBinary(std::string _filename, Animation& _animation)
{
	std::ifstream binaryRead;

	binaryRead.open(_filename.c_str(), std::ios_base::in | std::ios_base::binary);

	if (binaryRead.is_open() == true)
	{
		// Read The Length Of The Animation Name
		unsigned int length = 0;
		binaryRead.read((char*)&length, sizeof(unsigned int));

		// Read The Animation Name
		char* tempStr = new char[length + 1];
		binaryRead.read((char*)&tempStr[0], length);
		tempStr[length] = '\0';
		_animation.m_sName = tempStr;
		delete tempStr;

		// List Of Channels To Read
		std::vector<Channel> channels;

		// Read Channel Count
		unsigned int channelCount = 0;
		binaryRead.read((char*)&channelCount, sizeof(unsigned int));
		channels.reserve(channelCount);

		// For Each Channel
		for (unsigned int channel = 0; channel < channelCount; channel++)
		{
			Channel newChannel;

			// List Of Keys To Read
			std::vector<BoneKey> keys;

			// Read Key Count
			unsigned int keyCount = 0;
			binaryRead.read((char*)&keyCount, sizeof(unsigned int));
			keys.reserve(keyCount);

			// For Each Key
			for (unsigned int key = 0; key < keyCount; key++)
			{
				// The New Key To Filled Out
				BoneKey newKey;

				// Read This Keys Time
				binaryRead.read((char*)&newKey.time, sizeof(float));

				// Read This Keys Bone World Transform
				binaryRead.read((char*)&newKey.boneWorld, sizeof(Matrix4));

				// Read This Keys Bone World Offset Transform
				binaryRead.read((char*)&newKey.boneOffset, sizeof(Matrix4));

				// Add The New Key To The Keys List
				keys.push_back(newKey);
			}

			// Set This Channels Keys
			newChannel.keys = keys;

			// Add The New Channel To The Channels List
			channels.push_back(newChannel);
		}

		// Set This Animations Channels
		_animation.GetChannels() = channels;

		// Read Total Time
		float totalTime = 0.0f;
		binaryRead.read((char*)&totalTime, sizeof(float));
		_animation.SetDuration(totalTime);

		binaryRead.close();
	}
}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
void FBXLoader::WriteAnimationToBinary(std::string _filename, Animation& _animation)
{
	std::ofstream binaryWrite;

	binaryWrite.open(_filename.c_str(), std::ios_base::out | std::ios_base::binary);

	if (binaryWrite.is_open() == true)
	{
		// Write Animation Name Length
		const char* str = _animation.m_sName.c_str();
		unsigned int length = (unsigned int)strlen(str);
		binaryWrite.write((char*)&length, sizeof(unsigned int));

		// Write Animation Name
		binaryWrite.write(str, length);

		// Grab Copy Of The Animations Channels
		std::vector<Channel> channels = _animation.GetChannels();

		// Write Channel Count
		unsigned int channelCount = (unsigned int)channels.size();
		binaryWrite.write((char*)&channelCount, sizeof(unsigned int));

		// For Each Channel
		for (unsigned int channel = 0; channel < channelCount; channel++)
		{
			// Grab A Copy Of This Channels Keys
			std::vector<BoneKey> keys = channels[channel].keys;

			// Write Key Count
			unsigned int keyCount = (unsigned int)keys.size();
			binaryWrite.write((char*)&keyCount, sizeof(unsigned int));

			// For Each Key
			for (unsigned int key = 0; key < keyCount; key++)
			{
				// Write This Keys Time
				binaryWrite.write((char*)&keys[key].time, sizeof(float));

				// Write This Keys Bone World Transform
				binaryWrite.write((char*)&keys[key].boneWorld, sizeof(Matrix4));

				// Write This Keys Bone World Offset Transform
				binaryWrite.write((char*)&keys[key].boneOffset, sizeof(Matrix4));
			}
		}

		// Write Animation TotalTime
		float totalTime = _animation.GetDuration();
		binaryWrite.write((char*)&totalTime, sizeof(float));

		binaryWrite.close();
	}
}

//*********************************************************************//
//	In:
//
//	Out:	
//
//	Description:	
//*********************************************************************//
// bool FBXLoader::LoadLevelFromFBX(std::string _filename)
// {
// 	FbxManager* fbxManager = FbxManager::Create();
// 	if (fbxManager == NULL) return false;
// 
// 	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
// 	if (fbxIOSettings == NULL) return false;
// 
// 	fbxManager->SetIOSettings(fbxIOSettings);
// 
// 	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
// 	if (!fbxImporter) return false;
// 
// 	FbxStatus status = fbxImporter->GetStatus();
// 
// 	if (!ASSERT(fbxImporter->Initialize(_filename.c_str(), -1, fbxIOSettings), (std::string("FbxImporter::Initialize failed: ")
// 		+ fbxImporter->GetStatus().GetErrorString()).c_str()))
// 		return false;
// 
// 	status = fbxImporter->GetStatus();
// 
// 	FbxScene* fbxScene = FbxScene::Create(fbxManager, "myScene");
// 	if (fbxScene == NULL) return false;
// 
// 	if (fbxImporter->Import(fbxScene) == false)
// 	{
// 		status = fbxImporter->GetStatus();
// 		return false;
// 	}
// 
// 	fbxImporter->Destroy();
// 
// 	FbxGeometryConverter fbxGeometryConverter(fbxManager);
// 	if (fbxGeometryConverter.Triangulate(fbxScene, true, false) == false)
// 		return false;
// 
// 	if (fbxGeometryConverter.SplitMeshesPerMaterial(fbxScene, true) == false)
// 		return false;
// 
// 	std::unordered_map<std::string, std::vector<Matrix4>> levelData;
// 
// 	FbxNode* rootNode = fbxScene->GetRootNode();
// 	if (ProcessLevelMeshNode(rootNode, levelData) == false)
// 		return false;
// 
// 	 //ExportLevelData(levelData);
// 
// 	return true;
// }

#pragma region PRIVATE_METHODS

bool FBXLoader::ProcessMeshNode(FbxNode* _sceneNode, Mesh& _mesh, std::vector<FbxNode*>& _fbxBones)
{
	// TODO: This code only handles nodes that are relative to mesh.
	if (_sceneNode->GetNodeAttribute() != NULL && _sceneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* fbxMesh = _sceneNode->GetMesh();
		SubMesh newSubMesh;
		newSubMesh.name = _sceneNode->GetName();

		std::vector<LoadedVertex> vertices;
		std::vector<unsigned int> indices;

		if (LoadMeshData(fbxMesh, vertices, indices) == false)
			return false;

		std::vector<std::vector<BoneInfluence>> influences;
		influences.resize(vertices.size());

		FbxTime time = 0;
		FbxAMatrix currentWorld = GetGlobalTransform(_sceneNode, time);
		// Geometry offset is not inherited by the children.
		FbxAMatrix worldPosition = currentWorld * GetGeometryTranslation(_sceneNode);

		if (LoadMeshSkinData(fbxMesh, worldPosition, vertices, _mesh.GetBones(), influences, _fbxBones) == false)
			return false;

		RightToLeftHanded(indices);
		PadVerticeBoneInfluences(vertices, influences);

		newSubMesh.indices = indices;

		size_t vertCount = vertices.size();
		newSubMesh.vertices.resize(vertCount);

		for (size_t vert = 0; vert < vertCount; vert++)
		{
			newSubMesh.vertices[vert] = vertices[vert];
		}

		_mesh.GetSubMeshes().push_back(newSubMesh);
	}

	for (int child = 0; child < _sceneNode->GetChildCount(); child++)
		if (ProcessMeshNode(_sceneNode->GetChild(child), _mesh, _fbxBones) == false)
			return false;

	return true;
}

// bool FBXLoader::ProcessAnimMeshNode(FbxNode* _sceneNode, std::vector<FbxNode*>& _fbxBones)
// {
// 	unsigned int count = _sceneNode->GetChildCount();
// 
// 	if (_sceneNode->GetNodeAttribute() != NULL && _sceneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
// 	{
// 		FbxMesh* fbxMesh = _sceneNode->GetMesh();
// 
// 		LoadAnimSkinData(fbxMesh, _fbxBones);
// 	}
// 
// 	for (int child = 0; child < _sceneNode->GetChildCount(); child++)
// 		if (ProcessAnimMeshNode(_sceneNode->GetChild(child), _fbxBones) == false)
// 			return false;
// 
// 	return true;
// }

bool FBXLoader::ProcessMeshSkeletonNode(FbxNode* _clusterNode, std::vector<Bone>& _bones, std::vector<FbxNode*>& _fbxBones, int& _index)
{
	FbxString name = _clusterNode->GetName();

	if (_clusterNode->GetNodeAttribute() == nullptr || _clusterNode->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::EType::eSkeleton)
		return false;

	for (unsigned int joint = 0; joint < _fbxBones.size(); joint++)
	{
		if (_clusterNode->GetName() == _fbxBones[joint]->GetName())
		{
			_index = joint;

			return true;
		}
	}

	Bone newBone;
	newBone.m_Name = _clusterNode->GetName();

	_bones.push_back(newBone);

	_index = (unsigned int)_fbxBones.size();
	_fbxBones.push_back(_clusterNode);

	int childCount = _clusterNode->GetChildCount();

	for (int child = 0; child < childCount; child++)
	{
		int childIndex = 0;

		ProcessMeshSkeletonNode(_clusterNode->GetChild(child), _bones, _fbxBones, childIndex);

		newBone.GetChildrenIndices().push_back(childIndex);
	}

	if (_clusterNode->GetParent() != nullptr)
	{
		int parentIndex = -1;

		ProcessMeshSkeletonNode(_clusterNode, _bones, _fbxBones, parentIndex);

		newBone.SetParentIndex(parentIndex);
	}

	_bones[_index] = newBone;

	return true;
}

// bool FBXLoader::ProcessAnimSkeletonNode(FbxNode* _clusterNode, std::vector<FbxNode*>& _fbxBones)
// {
// 	if (_clusterNode->GetNodeAttribute() == nullptr || _clusterNode->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::EType::eSkeleton)
// 		return false;
// 
// 	for (unsigned int joint = 0; joint < _fbxBones.size(); joint++)
// 	{
// 		if (_clusterNode->GetName() == _fbxBones[joint]->GetName())
// 		{
// 			return true;
// 		}
// 	}
// 
// 	FbxString name = _clusterNode->GetName();
// 
// 	_fbxBones.push_back(_clusterNode);
// 
// 	int childCount = _clusterNode->GetChildCount();
// 
// 	for (int child = 0; child < childCount; child++)
// 		ProcessAnimSkeletonNode(_clusterNode->GetChild(child), _fbxBones);
// 
// 	if (_clusterNode->GetParent() != nullptr)
// 		ProcessAnimSkeletonNode(_clusterNode, _fbxBones);
// 
// 	return true;
// }

// bool FBXLoader::ProcessLevelMeshNode(FbxNode* _sceneNode, std::unordered_map<std::string, std::vector<Matrix4>>& _levelData)
// {
// 	if (_sceneNode->GetNodeAttribute() != NULL && _sceneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
// 	{
// 		//FbxMesh* fbxMesh = _sceneNode->GetMesh();
// 
// 		std::string meshName = _sceneNode->GetName();
// 		
// 		std::vector<std::string> strings = Inferno::StringParse(meshName, '_');
// 		meshName = strings[0] + '_' + strings[1];
// 
// 		FbxDouble3 translation = _sceneNode->LclTranslation.Get();
// 		FbxDouble3 rotation = _sceneNode->LclRotation.Get();
// 		FbxDouble3 scale = _sceneNode->LclScaling.Get();
// 
// 		FbxAMatrix fbxTransform = FbxAMatrix(translation, rotation, scale);
// 
// 		//FbxAMatrix fbxTransform = _sceneNode->GetAnimationEvaluator()->GetNodeGlobalTransform(_sceneNode);
// 
// 		Matrix4 transform = ConvertFBXMatrix(fbxTransform);
// 
// 		auto iter = _levelData.find(meshName);
// 		if (iter == _levelData.end())
// 			_levelData[meshName].push_back(transform);
// 		else
// 			iter->second.push_back(transform);
// 	}
// 
// 	for (int child = 0; child < _sceneNode->GetChildCount(); child++)
// 		if (ProcessLevelMeshNode(_sceneNode->GetChild(child), _levelData) == false)
// 			return false;
// 
// 	return true;
// }

bool FBXLoader::LoadMeshData(FbxMesh* _fbxMesh, std::vector<LoadedVertex>& _verticies, std::vector<unsigned int>& _indices)
{
	unsigned int numControlPoints = _fbxMesh->GetControlPointsCount();

	FbxVector4* fbxControlPoints = _fbxMesh->GetControlPoints();

	unsigned int fbxVertIndex = 0;

	std::unordered_map<LoadedVertex, int> uniqueVertList;

	for (int currentPolygon = 0; currentPolygon < _fbxMesh->GetPolygonCount(); currentPolygon++)
	{
		// Vert Data
		for (int currentVert = 0; currentVert < _fbxMesh->GetPolygonSize(currentPolygon); currentVert++)
		{
			LoadedVertex newVert;

			unsigned int controlPointIndex = _fbxMesh->GetPolygonVertex(currentPolygon, currentVert);

			// Position Data
			FbxVector4 translation = _fbxMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 rotation = _fbxMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 scale = _fbxMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix matrix = FbxAMatrix(translation, rotation, scale);

			FbxVector4 position = fbxControlPoints[controlPointIndex].mData;
			position = matrix.MultT(position);

			newVert.position.x = (float)position.mData[0];
			newVert.position.y = (float)position.mData[1];
			newVert.position.z = -(float)position.mData[2]; // Negate for right handed
			newVert.position.w = 1.0f;

			// Normal Data
			for (int currentNormal = 0; currentNormal < _fbxMesh->GetElementNormalCount(); currentNormal++)
			{
				FbxGeometryElementNormal* fbxNormal = _fbxMesh->GetElementNormal(currentNormal);

				FbxLayerElement::EMappingMode mappingMode = fbxNormal->GetMappingMode();
				FbxLayerElement::EReferenceMode referenceMode = fbxNormal->GetReferenceMode();

				int directArrayIndex = -1;

				if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (referenceMode == FbxGeometryElement::eDirect)
						directArrayIndex = fbxVertIndex;
					else if (referenceMode == FbxGeometryElement::eIndexToDirect)
						directArrayIndex = fbxNormal->GetIndexArray().GetAt(fbxVertIndex);
				}

				if (directArrayIndex != -1)
				{
					FbxVector4 normal = fbxNormal->GetDirectArray().GetAt(directArrayIndex);

					newVert.normal.x = (float)normal.mData[0];
					newVert.normal.y = (float)normal.mData[1];
					newVert.normal.z = -(float)normal.mData[2]; // Negate for right handed
				}
			}

			// Tangent Data
			for (int currentTangent = 0; currentTangent < _fbxMesh->GetElementTangentCount(); currentTangent++)
			{
				FbxGeometryElementTangent* fbxTangent = _fbxMesh->GetElementTangent(currentTangent);

				FbxLayerElement::EMappingMode mappingMode = fbxTangent->GetMappingMode();
				FbxLayerElement::EReferenceMode referenceMode = fbxTangent->GetReferenceMode();

				int directArrayIndex = -1;

				if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (referenceMode == FbxGeometryElement::eDirect)
						directArrayIndex = fbxVertIndex;
					else if (referenceMode == FbxGeometryElement::eIndexToDirect)
						directArrayIndex = fbxTangent->GetIndexArray().GetAt(fbxVertIndex);
				}

				if (directArrayIndex != -1)
				{
					FbxVector4 tangent = fbxTangent->GetDirectArray().GetAt(directArrayIndex);

					newVert.tangent.x = (float)tangent.mData[0];
					newVert.tangent.y = (float)tangent.mData[1];
					newVert.tangent.z = -(float)tangent.mData[2]; // Negate for right handed
				}
			}

			// UV Data
			for (int currentUV = 0; currentUV < _fbxMesh->GetElementUVCount(); currentUV++)
			{
				FbxGeometryElementUV* fbxUV = _fbxMesh->GetElementUV(currentUV);

				FbxLayerElement::EMappingMode mappingMode = fbxUV->GetMappingMode();
				FbxLayerElement::EReferenceMode referenceMode = fbxUV->GetReferenceMode();

				int directArrayIndex = -1;

				if (mappingMode == FbxGeometryElement::eByPolygonVertex)
				{
					if (referenceMode == FbxGeometryElement::eDirect || referenceMode == FbxGeometryElement::eIndexToDirect)
					{
						directArrayIndex = _fbxMesh->GetTextureUVIndex(currentPolygon, currentVert);
					}
				}
				else if (mappingMode == FbxGeometryElement::eByControlPoint)
				{
					if (referenceMode == FbxGeometryElement::eDirect)
						directArrayIndex = controlPointIndex;
					else if (referenceMode == FbxGeometryElement::eIndexToDirect)
						directArrayIndex = fbxUV->GetIndexArray().GetAt(controlPointIndex);
				}


				if (directArrayIndex != -1)
				{
					FbxVector4 uv = fbxUV->GetDirectArray().GetAt(directArrayIndex);

					newVert.uv.x = (float)uv.mData[0];
					newVert.uv.y = -(float)uv.mData[1]; // Negate Always

					//if (newVert.uv.x > 1.0f)
					//	newVert.uv.x -= 1.0f;
					//else if(newVert.uv.x < 0.0f)
					//	newVert.uv.x += 1.0f;

					//if (newVert.uv.y > 1.0f)
					//	newVert.uv.y -= 1.0f;
					//else if (newVert.uv.y < 0.0f)
					//	newVert.uv.y += 1.0f;
				}
			}

			// Control Point (for Skin Extraction)
			newVert.controlPointIndex = controlPointIndex;

			// Unique Vert Check
			auto iter = uniqueVertList.find(newVert);
			if (iter == uniqueVertList.end())
			{
				_indices.push_back((unsigned int)uniqueVertList.size());
				uniqueVertList.insert(std::pair<LoadedVertex, int>(newVert, (int)uniqueVertList.size()));
				_verticies.push_back(newVert);
			}
			else
				_indices.push_back(iter->second);

			fbxVertIndex++;
		}
	}

	return true;
}

bool FBXLoader::LoadMeshSkinData(FbxMesh* _fbxMesh, FbxAMatrix& _meshWorld, std::vector<LoadedVertex>& _vertices, std::vector<Bone>& _bones, std::vector<std::vector<BoneInfluence>>& _influences, std::vector<FbxNode*> _fbxBones)
{
	int deformerCount = _fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformer = 0; deformer < deformerCount; deformer++)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(_fbxMesh->GetDeformer(deformer, FbxDeformer::eSkin));

		int clusterCount = currSkin->GetClusterCount();

		for (int cluster = 0; cluster < clusterCount; cluster++)
		{
			int controlPointIndex = 0;

			ProcessMeshSkeletonNode(currSkin->GetCluster(cluster)->GetLink(), _bones, _fbxBones, controlPointIndex);

			// Matrix4 globalBindPose = GetGlobalTransform(currSkin->GetCluster(cluster)->GetLink(), currSkin->GetCluster(cluster));
			// Matrix4 boneOffset = globalBindPose * globalBindPose.Inverse();
			// 
			// _bones[controlPointIndex].SetBindPose(globalBindPose);
			// _bones[controlPointIndex].SetInverseGlobalBindPose(globalBindPose.Inverse());
			// _bones[controlPointIndex].SetBoneOffset(boneOffset);

			FbxTime time = 0;
			FbxAMatrix boneWorld, boneOffset;
			GetBoneInfo(_fbxMesh, currSkin->GetCluster(cluster), _meshWorld, time, boneWorld, boneOffset);

			_bones[controlPointIndex].SetBindPose(ConvertFBXMatrix(boneWorld));
			_bones[controlPointIndex].SetInverseGlobalBindPose(ConvertFBXMatrix(boneWorld.Inverse()));
			_bones[controlPointIndex].SetBoneOffset(ConvertFBXMatrix(boneOffset));

			int controlPointIndiceCount = currSkin->GetCluster(cluster)->GetControlPointIndicesCount();

			for (int controlPoint = 0; controlPoint < controlPointIndiceCount; controlPoint++)
			{
				BoneInfluence newInfluence;
				newInfluence.boneIndex = (float)controlPointIndex;
				newInfluence.boneWeight = (float)currSkin->GetCluster(cluster)->GetControlPointWeights()[controlPoint];

				size_t vertCount = _vertices.size();

				for (size_t vert = 0; vert < vertCount; vert++)
				{
					if (_vertices[vert].controlPointIndex == currSkin->GetCluster(cluster)->GetControlPointIndices()[controlPoint])
					{
						_influences[vert].push_back(newInfluence);
					}
				}
			}
		}
	}

	return true;
}

void FBXLoader::CreateBoundingSphere(Mesh& _mesh)
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = FBXSDK_FLOAT_MAX;
	maxX = maxY = maxZ = FBXSDK_FLOAT_MIN;

	std::vector<SubMesh> subMeshes = _mesh.GetSubMeshes();
	size_t subMeshCount = subMeshes.size();
	for (size_t subMeshIndex = 0; subMeshIndex < subMeshCount; subMeshIndex++)
	{
		std::vector<Vertex> verts = subMeshes[subMeshIndex].vertices;
		size_t vertCount = verts.size();
		for (size_t vertIndex = 0; vertIndex < vertCount; vertIndex++)
		{
			Vector3 vertPos = verts[vertIndex].position;

			if (vertPos.x < minX)
				minX = vertPos.x;
			if (vertPos.x > maxX)
				maxX = vertPos.x;

			if (vertPos.y < minY)
				minY = vertPos.y;
			if (vertPos.y > maxY)
				maxY = vertPos.y;

			if (vertPos.z < minZ)
				minZ = vertPos.z;
			if (vertPos.z > maxZ)
				maxZ = vertPos.z;
		}
	}

	Vector3 center = Vector3((minX + maxX) * 0.5f, (minX + maxX) * 0.5f, (minX + maxX) * 0.5f);
	float radius = max(max(maxX - minX, maxY - minY), maxZ - minZ) * 0.5f;

	Sphere boundingSphere;
	boundingSphere.center = center;
	boundingSphere.radius = radius;

	_mesh.SetBoundingSpehere(boundingSphere);
}

bool FBXLoader::LoadAnimation(FbxAnimLayer* _animLayer, FbxNode* _node, Animation& _animation/*, std::vector<FbxNode*>& fbxJoints*/)
{
	if (_node->GetNodeAttribute() != NULL)
	{
		FbxTime time = 0;
		FbxAMatrix currentWorld = GetGlobalTransform(_node, time);

		// Geometry offset is not inherited by the children.
		FbxAMatrix worldPosition = currentWorld * GetGeometryTranslation(_node);

		LoadAnimation(_animLayer, _node, worldPosition, _animation);
	}

	const int childCount = _node->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; childIndex++)
	{
		LoadAnimation(_animLayer, _node->GetChild(childIndex), _animation);
	}

	// for (unsigned int currentJoint = 0; currentJoint < fbxJoints.size(); currentJoint++)
	// {
		//std::vector<float> keyTimes;

		// LoadCurve(fbxJoints[currentJoint]->LclTranslation.GetCurve(animLayer), keyTimes);
		// LoadCurve(fbxJoints[currentJoint]->LclRotation.GetCurve(animLayer), keyTimes);
		// LoadCurve(fbxJoints[currentJoint]->LclScaling.GetCurve(animLayer), keyTimes);
		   
		// Channel channel;
		// for (unsigned int keyTimeIndex = 1; keyTimeIndex < keyTimes.size(); keyTimeIndex++)
		// {
		// 	BoneKey newKey;
		// 	FbxTime keyTime;
		// 	keyTime.SetSecondDouble(keyTimes[keyTimeIndex]);
		// 	newKey.time = keyTimes[keyTimeIndex];
		   
		// 	FbxAMatrix keyWorld = fbxJoints[currentJoint]->EvaluateGlobalTransform(keyTime);
		// 	Matrix4 boneTransform = ConvertFBXMatrix(keyWorld);
		   
		// 	newKey.transform = boneTransform;
		   
		// 	channel.keys.push_back(newKey);
		// }
	// }

	return true;
}

bool FBXLoader::LoadAnimation(FbxAnimLayer* _animLayer, FbxNode* _node, FbxAMatrix& _meshWorld, Animation& _animation)
{
		FbxNodeAttribute* attribute = _node->GetNodeAttribute();

		if (attribute != NULL && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			LoadAnimationData(_animLayer, _node->GetMesh(), _meshWorld, _animation);
		}

		return true;
}

void FBXLoader::LoadAnimationData(FbxAnimLayer* animLayer, FbxMesh* _mesh, FbxAMatrix& _meshWorld, Animation& _animation)
{
	int deformerCount = _mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; deformerIndex++)
	{
		FbxSkin* currentDeformer = reinterpret_cast<FbxSkin*>(_mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		//FbxSkin* currentDeformer = static_cast<FbxSkin *>(_mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		int clusterCount = currentDeformer->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
		{
			FbxCluster* currentCluster = currentDeformer->GetCluster(clusterIndex);
			FbxString name = currentCluster->GetLink()->GetName();

			if (!currentCluster->GetLink())
				continue;

			std::vector<float> keyTimes;

			LoadCurve(currentCluster->GetLink()->LclTranslation.GetCurve(animLayer), keyTimes);
			LoadCurve(currentCluster->GetLink()->LclRotation.GetCurve(animLayer), keyTimes);
			LoadCurve(currentCluster->GetLink()->LclScaling.GetCurve(animLayer), keyTimes);

			Channel channel;
			// Maybe set key time index to 1?
			for (unsigned int keyTimeIndex = 0; keyTimeIndex < keyTimes.size(); keyTimeIndex++)
			{
				BoneKey newKey;
				FbxTime keyTime;
				keyTime.SetSecondDouble(keyTimes[keyTimeIndex]);
				newKey.time = keyTimes[keyTimeIndex];

				FbxAMatrix keyWorld, keyOffset;
				GetBoneInfo(_mesh, currentCluster, _meshWorld, keyTime, keyWorld, keyOffset);
				newKey.boneWorld = ConvertFBXMatrix(keyWorld);
				newKey.boneOffset = ConvertFBXMatrix(keyOffset);

				channel.keys.push_back(newKey);
			}

			_animation.GetChannels().push_back(channel);
		}
	}
}

// bool FBXLoader::LoadAnimSkinData(FbxMesh* _fbxMesh, std::vector<FbxNode*>& _fbxBones)
// {
// 	int deformerCount = _fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
// 
// 	for (int deformer = 0; deformer < deformerCount; deformer++)
// 	{
// 		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(_fbxMesh->GetDeformer(deformer, FbxDeformer::eSkin));
// 
// 		int clusterCount = currSkin->GetClusterCount();
// 		for (int cluster = 0; cluster < clusterCount; cluster++)
// 		{
// 			int controlPointIndex = 0;
// 
// 			ProcessAnimSkeletonNode(currSkin->GetCluster(cluster)->GetLink(), _fbxBones);
// 		}
// 	}
// 
// 	return true;
// }

void FBXLoader::LoadCurve(FbxAnimCurve* animCurve, std::vector<float>& keyTimes)
{
	if (animCurve == NULL) return;

	int keyCount = animCurve->KeyGetCount();
	for (int currKey = 0; currKey < keyCount; currKey++)
	{
		bool unique = true;

		for (unsigned int keyTime = 0; keyTime < keyTimes.size(); keyTime++)
		{
			if (keyTimes[keyTime] == (float)animCurve->KeyGetTime(currKey).GetSecondDouble())
			{
				unique = false;
				break;
			}
		}

		if (unique == true)
		{
			keyTimes.push_back((float)animCurve->KeyGetTime(currKey).GetSecondDouble());
		}
	}
}

// void FBXLoader::ExportLevelData(std::unordered_map<std::string, std::vector<Matrix4>>& _levelData)
// {
// 	// Get mesh count
// 	size_t meshCount = _levelData.size();
// 
// 	// TODO:
// 	// Write mesh count
// 
// 
// 	// Iterate through level data map
// 	auto iter = _levelData.begin();
// 	while (iter != _levelData.end())
// 	{
// 		// Get mesh name
// 		std::string meshName = iter->first;
// 
// 		// TODO:
// 		// Write mesh name
// 
// 
// 		// TODO:
// 		// Write Faction (Enemy, Friendly, Neutral)
// 
// 
// 		// Get transform count
// 		size_t transformCount = iter->second.size();
// 
// 		// TODO:
// 		// Write transform count
// 
// 		// For each transform
// 		for (size_t currTransform = 0; currTransform < transformCount; currTransform++)
// 		{
// 			// Get transform data
// 			Matrix4 transform = iter->second[currTransform];
// 
// 			// TODO:
// 			// Write transform data
// 
// 		}
// 
// 		// Next mesh
// 		++iter;
// 	}
// }

Matrix4 FBXLoader::ConvertFBXMatrix(const FbxAMatrix& _fbxMatrix)
{
	Matrix4 result;

	result._11 = (float)_fbxMatrix.Get(0, 0);
	result._12 = (float)_fbxMatrix.Get(0, 1);
	result._13 = -(float)_fbxMatrix.Get(0, 2); // negate for right handed
	result._14 = (float)_fbxMatrix.Get(0, 3);

	result._21 = (float)_fbxMatrix.Get(1, 0);
	result._22 = (float)_fbxMatrix.Get(1, 1);
	result._23 = -(float)_fbxMatrix.Get(1, 2); // negate for right handed
	result._24 = (float)_fbxMatrix.Get(1, 3);

	result._31 = -(float)_fbxMatrix.Get(2, 0); // negate for right handed
	result._32 = -(float)_fbxMatrix.Get(2, 1); // negate for right handed
	result._33 = (float)_fbxMatrix.Get(2, 2);
	result._34 = (float)_fbxMatrix.Get(2, 3);

	result._41 = (float)_fbxMatrix.Get(3, 0);
	result._42 = (float)_fbxMatrix.Get(3, 1);
	result._43 = -(float)_fbxMatrix.Get(3, 2); // negate for right handed
	result._44 = (float)_fbxMatrix.Get(3, 3);

	return result;
}

void FBXLoader::GetBoneInfo(FbxMesh* _mesh, FbxCluster* _cluster, FbxAMatrix& _meshWorld, FbxTime _time, FbxAMatrix& _boneWorld, FbxAMatrix& _boneOffset)
{
	FbxAMatrix meshBindPose;
	FbxAMatrix bindPose;

	//Scale and positional data only
	_cluster->GetTransformMatrix(meshBindPose);
	//Like getting the plug in to the bind pose plug-in in the maya API
	_cluster->GetTransformLinkMatrix(bindPose);
	//Get the bone as it is in world space right now
	_boneWorld = GetGlobalTransform(_cluster->GetLink(), _time);

	_boneOffset = _meshWorld.Inverse() * _boneWorld * bindPose.Inverse() * meshBindPose * GetGeometryTranslation(_mesh->GetNode());
}

FbxAMatrix FBXLoader::GetLocalTransform(FbxNode* _meshNode, FbxTime _time)
{
	return _meshNode->EvaluateLocalTransform(_time);
}

FbxAMatrix FBXLoader::GetGlobalTransform(FbxNode* _meshNode, FbxTime _time/*, FbxCluster* _cluster*/)
{
	//FbxVector4 translation = _meshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	//FbxVector4 rotation = _meshNode->GetGeometricRotation(FbxNode::eSourcePivot);
	//FbxVector4 scaling = _meshNode->GetGeometricScaling(FbxNode::eSourcePivot);
	//FbxAMatrix geometryTransform = FbxAMatrix(translation, rotation, scaling);
	//
	//FbxAMatrix transformLinkMatrix;
	//_cluster->GetTransformLinkMatrix(transformLinkMatrix);
	//
	//FbxAMatrix transformMatrix;
	//_cluster->GetTransformMatrix(transformMatrix);
	//
	//FbxAMatrix result = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

	//return ConvertFBXMatrix(_meshNode->EvaluateGlobalTransform());

	FbxAMatrix globalTransform = _meshNode->EvaluateGlobalTransform(_time);
	return globalTransform;
}

FbxAMatrix FBXLoader::GetGeometryTranslation(FbxNode* _meshNode)
{
	FbxVector4 translation = _meshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rotation = _meshNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scaling = _meshNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryTransform = FbxAMatrix(translation, rotation, scaling);

	return geometryTransform;
}

void FBXLoader::PadVerticeBoneInfluences(std::vector<LoadedVertex>& _verts, std::vector<std::vector<BoneInfluence>>& _influences)
{
	size_t vertCount = _verts.size();

	for (size_t vert = 0; vert < vertCount; vert++)
	{
		size_t influenceCount = _influences[vert].size();

		for (size_t influence = 0; influence < influenceCount; influence++)
		{
			switch (influence)
			{
			case 0:
			{
				_verts[vert].boneIndices.x = _influences[vert][influence].boneIndex;
				_verts[vert].boneWeights.x = _influences[vert][influence].boneWeight;
			}
			break;
			case 1:
			{
				_verts[vert].boneIndices.y = _influences[vert][influence].boneIndex;
				_verts[vert].boneWeights.y = _influences[vert][influence].boneWeight;
			}
			break;
			case 2:
			{
				_verts[vert].boneIndices.z = _influences[vert][influence].boneIndex;
				_verts[vert].boneWeights.z = _influences[vert][influence].boneWeight;
			}
			break;
			case 3:
			{
				_verts[vert].boneIndices.w = _influences[vert][influence].boneIndex;
				_verts[vert].boneWeights.w = _influences[vert][influence].boneWeight;
			}
			break;
			default:
			{

			}
			break;
			}
		}
	}
}

void FBXLoader::RightToLeftHanded(std::vector<unsigned int>& _indices)
{
	for (unsigned int index = 1; index < _indices.size(); index += 3)
		std::swap(_indices[index], _indices[index + 1]);
}

#pragma endregion
