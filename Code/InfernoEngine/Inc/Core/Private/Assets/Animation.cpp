//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	Animation holds onto skinned animation data
//*********************************************************************//

#include "Assets/Animation.h"

Animation::Animation()
{
	m_fDuration = 0.0f;
	m_bIsLooping = false;
	m_bIsReversed = false;
}

Animation::~Animation()
{

}

void Animation::AddChannel(Channel _channel)
{
	m_Channels.push_back(_channel);
}

std::vector<Channel>& Animation::GetChannels()
{
	return m_Channels;
}

float Animation::GetDuration()
{
	return m_fDuration;
}

bool Animation::IsLooping()
{
	return m_bIsLooping;
}

bool Animation::IsReversed()
{
	return m_bIsReversed;
}

void Animation::SetDuration(float _time)
{
	m_fDuration = _time;
}

void Animation::SetLooping(bool _looping)
{
	m_bIsLooping = _looping;
}

void Animation::SetReversed(bool _reversed)
{
	m_bIsReversed = _reversed;
}

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
//void Animation::Update(std::vector<Bone>& _bones, std::vector<Matrix4>& _boneOffsets, float _time)
//{
//	if (_time < 0.0f)
//		_time += m_fDuration;
//	else if (_time >= m_fDuration)
//		_time -= m_fDuration;
//
//	size_t channelCount = m_Channels.size();
//	for (size_t channel = 0; channel < channelCount; channel++)
//	{
//		if (channel >= _bones.size())
//			continue;
//
//		int frame = 0, nextFrame = 0;
//
//		if (m_Channels[channel].keys.size() == 0)
//			continue;
//
//		if (m_Channels[channel].keys.size() == 1)
//		{
//			Matrix4 newBoneOffset = m_Channels[channel].keys[0].transform * _bones[channel].GetInverseGlobalBindPose();
//			_boneOffsets[channel] = newBoneOffset;
//			continue;
//		}
//
//		if (_time < m_Channels[channel].keys[0].time || _time > m_Channels[channel].keys[m_Channels[channel].keys.size() - 1].time)
//		{
//			frame = (int)m_Channels[channel].keys.size() - 1;
//		}
//		else
//		{
//			nextFrame++;
//
//			while (true)
//			{
//				if (m_Channels[channel].keys[frame].time <= _time && m_Channels[channel].keys[nextFrame].time > _time)
//					break;
//
//				frame++, nextFrame++;
//			}
//		}
//
//		float interpRatio;
//
//		if (frame < nextFrame)
//		{
//			float totalTime = m_Channels[channel].keys[nextFrame].time - m_Channels[channel].keys[frame].time;
//			interpRatio = (_time - m_Channels[channel].keys[frame].time) / totalTime;
//		}
//		else
//		{
//			interpRatio = _time / m_Channels[channel].keys[nextFrame].time;
//		}
//
//		Matrix4 Key1 = _bones[channel].GetInverseGlobalBindPose() * m_Channels[channel].keys[frame].transform;
//		Matrix4 Key2 = _bones[channel].GetInverseGlobalBindPose() * m_Channels[channel].keys[nextFrame].transform;
//			
//		_boneOffsets[channel] = (Key1 * (1.0f - interpRatio) + Key2 * interpRatio);
//	}
//}