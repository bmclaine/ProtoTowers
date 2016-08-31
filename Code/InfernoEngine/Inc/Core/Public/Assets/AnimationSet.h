//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "Assets\Animation.h"

#include "GlobalIncludes.h"

class AnimationSet
{
public:
	std::string m_sName;

	AnimationSet();
	AnimationSet(unsigned int _animCount);

	~AnimationSet();

	void AddAnimation(Animation& _animation);
	void AddBlendTime(unsigned int _index1, unsigned int _index2, float _blendTime);
	
	Animation* GetAnimation(std::string& _animName);
	Animation* GetAnimation(size_t _index);

	std::vector<Animation>& GetAnimations();

	const float GetBlendTime(std::string& _fromAnimName, std::string& _toAnimName);

	Animation* GetDefaultAnimation();

	void SetDefaultAnimation(std::string& _animName);

	void ReserveSpace(unsigned int _animCount);

	bool CheckForAnimation(std::string& _animName);

private:
	std::vector<Animation> m_vAnimations;

	std::vector<std::vector<float>> m_vBlendTimes;

	unsigned int m_uiDefaultAnimation;
};