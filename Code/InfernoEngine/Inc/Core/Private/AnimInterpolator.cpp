//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "AnimInterpolator.h"

#include <DirectXMath.h>
using namespace DirectX;

AnimInterpolator::AnimInterpolator()
{
	m_pAnimation = nullptr;
	m_fCurrTime = 0.0f;
}

AnimInterpolator::~AnimInterpolator()
{
	m_pAnimation = nullptr;
	m_vPose.clear();
}

void AnimInterpolator::SetAnimation(Animation* _animation)
{
	m_pAnimation = _animation;
	m_fCurrTime = 0.0f;
	m_vPose.resize(_animation->GetChannels().size());
}

Animation* AnimInterpolator::GetAnimation()
{
	return m_pAnimation;
}

void AnimInterpolator::AddTime(float _time)
{
	SetTime(m_fCurrTime + _time);
}

void AnimInterpolator::SetTime(float _time)
{
	if (m_pAnimation != nullptr)
	{
		// If we're looping
		//if (m_pAnimation->IsLooping() == true)
		//{
			// Adjust time apporpriately
			if (_time < 0.0f)
			{
				m_fCurrTime = _time + m_pAnimation->GetDuration();
			}
			else if (_time > m_pAnimation->GetDuration())
			{
				m_fCurrTime = _time - m_pAnimation->GetDuration();
			}
			else
				m_fCurrTime = _time;
		//}
		//else // if we're not looping
		//{
		//	// and we're not finished
		//	if (IsFinished() == false)
		//		m_fCurrTime = _time;
		//	else

		//}
	}
}

float AnimInterpolator::GetTime()
{
	return m_fCurrTime;
}

bool AnimInterpolator::IsFinished()
{
	if (m_pAnimation != nullptr)
	{
		return m_fCurrTime >= m_pAnimation->GetDuration();
	}

	return true;
}

bool AnimInterpolator::IsPlaying()
{
	if (m_pAnimation != nullptr)
	{
		return m_fCurrTime < m_pAnimation->GetDuration();
	}

	return false;
}

void AnimInterpolator::ProcessAnimation(std::vector<Bone>& _bindpose)
{
	if (m_pAnimation == nullptr)
	{
		m_vPose.clear();
		return;
	}

	if (m_pAnimation->GetChannels().size() > 0)
	{
		ProcessChannels(m_pAnimation->GetChannels(), _bindpose);
	}
}

std::vector<Matrix4>& AnimInterpolator::GetPose()
{
	return m_vPose;
}

void AnimInterpolator::ProcessChannels(std::vector<Channel>& _channels, std::vector<Bone>& _bindpose)
{
	size_t channelCount = _channels.size();
	m_vPose.resize(channelCount);
	for (size_t channelIndex = 0; channelIndex < channelCount; channelIndex++)
	{
		//if (channelIndex >= _bindpose.size())
		//	continue;
		//
		//int frame = 0, nextFrame = 0;
		//
		//if (_channels[channelIndex].keys.size() == 0)
		//	continue;
		//
		//if (_channels[channelIndex].keys.size() == 1)
		//{
		//	Matrix4 newBoneOffset = _channels[channelIndex].keys[0].transform * _bindpose[channelIndex].GetInverseGlobalBindPose();
		//	m_vPose[channelIndex] = newBoneOffset;
		//	continue;
		//}
		//
		//if (m_fCurrTime < _channels[channelIndex].keys[0].time || m_fCurrTime > _channels[channelIndex].keys[_channels[channelIndex].keys.size() - 1].time)
		//{
		//	frame = (int)_channels[channelIndex].keys.size() - 1;
		//}
		//else
		//{
		//	nextFrame++;
		//
		//	while (true)
		//	{
		//		if (_channels[channelIndex].keys[frame].time <= m_fCurrTime && _channels[channelIndex].keys[nextFrame].time > m_fCurrTime)
		//			break;
		//
		//		frame++, nextFrame++;
		//	}
		//}
		//
		//float interpRatio;
		//
		//if (frame < nextFrame)
		//{
		//	float totalTime = _channels[channelIndex].keys[nextFrame].time - _channels[channelIndex].keys[frame].time;
		//	interpRatio = (m_fCurrTime - _channels[channelIndex].keys[frame].time) / totalTime;
		//}
		//else
		//{
		//	interpRatio = m_fCurrTime / _channels[channelIndex].keys[nextFrame].time;
		//}

		int keyCount = (int)_channels[channelIndex].keys.size();
		int currKey = 0, nextKey = 1;
		for (int keyIndex = 0; keyIndex < keyCount; keyIndex++)
		{
			if (m_fCurrTime < _channels[channelIndex].keys[keyIndex].time)
			{
				if (keyIndex != 0)
				{
					currKey = keyIndex - 1;
					nextKey = keyIndex;
				}
				else
				{
					currKey = keyCount - 1;
					nextKey = 0;
				}
				break;
			}
		}
		float lerpRatio;
		if (nextKey == 0)
		{
			lerpRatio = m_fCurrTime / _channels[channelIndex].keys[nextKey].time;
		}
		else
		{
			float tweenTime = _channels[channelIndex].keys[nextKey].time - _channels[channelIndex].keys[currKey].time;
			float frameTime = m_fCurrTime - _channels[channelIndex].keys[currKey].time;
			lerpRatio = frameTime / tweenTime;
		}

		Matrix4 Key1 = /*_bindpose[channelIndex].GetInverseGlobalBindPose()**/ _channels[channelIndex].keys[currKey].boneOffset;
		Matrix4 Key2 = /*_bindpose[channelIndex].GetInverseGlobalBindPose()**/ _channels[channelIndex].keys[nextKey].boneOffset;

		//XMMATRIX mat1 = XMLoadFloat4x4(&Key1);
		//XMVECTOR trans1, rot1, scale1;
		//XMMatrixDecompose(&scale1, &rot1, &trans1, mat1);

		//XMMATRIX mat2 = XMLoadFloat4x4(&Key2);
		//XMVECTOR trans2, rot2, scale2;
		//XMMatrixDecompose(&scale2, &rot2, &trans2, mat2);

		//XMVECTOR finalTrans = XMVectorLerp(trans1, trans2, lerpRatio);
		//XMVECTOR finalRot = XMQuaternionSlerp(rot1, rot2, lerpRatio);
		//XMVECTOR finalScale = XMVectorLerp(scale1, scale2, lerpRatio);

		//XMMATRIX finalMat = XMMatrixAffineTransformation(finalScale, XMVectorZero(), finalRot, finalTrans);

		//XMStoreFloat4x4(&m_vPose[channelIndex], finalMat);

		// Linear Interpolation. Not Good. Use DirectX Math.
		m_vPose[channelIndex] = (Key1 * (1.0f - lerpRatio) + Key2 * lerpRatio);
	}
}