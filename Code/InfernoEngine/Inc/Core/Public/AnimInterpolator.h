//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "Assets\Animation.h"
#include "Assets\Bone.h"

class AnimInterpolator
{
public:
	AnimInterpolator();
	~AnimInterpolator();

	void SetAnimation(Animation* _animation);
	Animation* GetAnimation();

	void AddTime(float _time);
	void SetTime(float _time);
	float GetTime();

	bool IsFinished();
	bool IsPlaying();

	void ProcessAnimation(std::vector<Bone>& _bindpose);

	std::vector<Matrix4>& GetPose();

private:
	void ProcessChannels(std::vector<Channel>& _channels, std::vector<Bone>& _bindpose);

	Animation* m_pAnimation;

	float m_fCurrTime;

	std::vector<Matrix4> m_vPose;

};