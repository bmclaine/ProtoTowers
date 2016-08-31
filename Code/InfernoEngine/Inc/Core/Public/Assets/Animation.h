//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Animation holds onto skinned animation data
//*********************************************************************//
#pragma once

#include "MathLib.h"
#include "GlobalIncludes.h"
#include "Bone.h"

#include <unordered_map>

class SkinnedMeshRenderer;

struct BoneKey
{
	float time;
	Matrix4 boneWorld;
	Matrix4 boneOffset;
};

struct Channel
{
	std::vector<BoneKey> keys;
};

class Animation
{
public:

	Animation();
	~Animation();

	void AddChannel(Channel _channel);

	std::vector<Channel>& GetChannels();

	float GetDuration();

	bool IsLooping();
	bool IsReversed();

	void SetDuration(float _time);
	void SetLooping(bool _looping);
	void SetReversed(bool _reversed);

	//*********************************************************************//
	//	In:				_bones - The bones to updated by this animation
	//					_time - The time that has passed since the animation
	//							started playing
	//					
	//	Out:			void
	//					
	//	Description:	Updates the RenderMesh Bone that are passed in
	//					to the correct pose.
	//*********************************************************************//
	//void Update(std::vector<Bone>& _bones, std::vector<Matrix4>& _boneOffsets, float _time);

	std::string m_sName;

private:
	std::vector<Channel> m_Channels;

	float m_fDuration;

	bool m_bIsLooping;

	bool m_bIsReversed;
};

