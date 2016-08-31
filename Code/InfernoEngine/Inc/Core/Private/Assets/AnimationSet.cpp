//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "Assets\AnimationSet.h"

AnimationSet::AnimationSet()
{
	m_uiDefaultAnimation = 0;
}

AnimationSet::AnimationSet(unsigned int _animCount)
{
	m_vAnimations.reserve(_animCount);
	m_uiDefaultAnimation = 0;
}

AnimationSet::~AnimationSet()
{

}

void AnimationSet::AddAnimation(Animation& _animation)
{
	// Check to see if the animation is already in the set..
	auto iter = m_vAnimations.begin();
	while (iter != m_vAnimations.end())
	{
		// If we already have this animation in the set, don't add it.
		if (iter->m_sName == _animation.m_sName)
			return;

		++iter;
	}

	// If we made it through our check, the animation is new, add it.
	m_vAnimations.push_back(_animation);
}

void AnimationSet::AddBlendTime(unsigned int _index1, unsigned int _index2, float _blendTime)
{
	m_vBlendTimes[_index1][_index2] = _blendTime;
}

Animation* AnimationSet::GetAnimation(std::string& _animName)
{
	auto iter = m_vAnimations.begin();
	while (iter != m_vAnimations.end())
	{
		if (iter->m_sName == _animName)
			return &(*iter);

		++iter;
	}

	return nullptr;
}

Animation* AnimationSet::GetAnimation(size_t _index)
{
	return &m_vAnimations[_index];
}

std::vector<Animation>& AnimationSet::GetAnimations()
{
	return m_vAnimations;
}

const float AnimationSet::GetBlendTime(std::string& _fromAnimName, std::string& _toAnimName)
{
	unsigned int fromIndex = 0, toIndex = 0, loopIndex = 0;

	auto iter = m_vAnimations.begin();
	while (iter != m_vAnimations.end())
	{
		if (iter->m_sName == _fromAnimName)
			fromIndex = loopIndex;

		if (iter->m_sName == _toAnimName)
			toIndex = loopIndex;

		++loopIndex;
		++iter;
	}

	return m_vBlendTimes[fromIndex][toIndex];
}

Animation* AnimationSet::GetDefaultAnimation()
{
	return &(m_vAnimations[m_uiDefaultAnimation]);
}

void AnimationSet::SetDefaultAnimation(std::string& _animName)
{
	unsigned int index = 0;
	auto iter = m_vAnimations.begin();
	while (iter != m_vAnimations.end())
	{
		if (iter->m_sName == _animName)
		{
			m_uiDefaultAnimation = index;
			return;
		}

		++index;
		++iter;
	}
}

void AnimationSet::ReserveSpace(unsigned int _animCount)
{
	m_vAnimations.reserve(_animCount);
	m_vBlendTimes.resize(_animCount);

	for (unsigned int anim = 0; anim < _animCount; anim++)
	{
		m_vBlendTimes[anim].resize(_animCount);
	}
}

bool AnimationSet::CheckForAnimation(std::string& _animName)
{
	auto iter = m_vAnimations.begin();
	while (iter != m_vAnimations.end())
	{
		if (iter->m_sName == _animName)
			return true;

		++iter;
	}

	return false;
}