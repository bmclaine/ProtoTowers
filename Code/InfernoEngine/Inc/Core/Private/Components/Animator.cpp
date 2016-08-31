//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//

#include "Components\Animator.h"
#include "Components\SkinnedMeshRenderer.h"
#include "InfernoTime.h"
#include "../../Interface/Public/Engine.h"
#include "Assets\AnimationController.h"
#include "Components\Transform.h"
#include "GameObject.h"

#include <DirectXMath.h>
using namespace DirectX;

Animator::Animator(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pAnimController = nullptr;

	m_uiCurrInterpolator = 0;

	m_fSpeed = 1.0f;

	m_fBlendTime = 0.0f;
	m_fElapsedBlendTime = 0.0f;

	// TODO::DELETE
	// m_pAnimation = nullptr;
	// m_fCurrentTime = 0.0f;
	// TODO::END
}

Animator::~Animator()
{

}

IComponent& Animator::operator=(IComponent& _rhs)
{
	Animator* rhsAnimator = (Animator*)&_rhs;
	if (rhsAnimator == nullptr)
	{
		return *this;
	}

	return operator=(*rhsAnimator);
}

Animator& Animator::operator=(const Animator& _rhs)
{
	// === Copy over the Members
	m_pRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
	m_pAnimController = _rhs.m_pAnimController;

	m_aInterpolators[0] = _rhs.m_aInterpolators[0];
	m_aInterpolators[1] = _rhs.m_aInterpolators[1];

	m_vPose = _rhs.m_vPose;

	m_uiCurrInterpolator = _rhs.m_uiCurrInterpolator;

	// TODO::DELETE
	// m_pAnimation = _rhs.m_pAnimation;
	// m_fCurrentTime = _rhs.m_fCurrentTime;
	// TODO::END

	m_fBlendTime = _rhs.m_fBlendTime;
	m_fElapsedBlendTime = _rhs.m_fElapsedBlendTime;

	m_fcFlags = _rhs.m_fcFlags;

	return *this;
}

void Animator::Init()
{
	m_pRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
}

void Animator::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "Controller";
	Property* prop = data->GetProperty(propertyName);
	SetAnimationController(Inferno::GetAnimationControllerByName(prop->value.c_str()));

	m_pRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
}

IComponent* Animator::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	Animator* anim = new Animator(gameObject, transform);
	(*anim) = *this;

	return anim;
}

std::string& Animator::GetCurrentAnimation()
{
	return m_aInterpolators[m_uiCurrInterpolator].GetAnimation()->m_sName;
}

AnimationController* Animator::GetAnimationController() const
{
	return m_pAnimController;
}

std::vector<Matrix4>& Animator::GetPose()
{
	return m_vPose;
}

float Animator::GetSpeed()
{
	return m_fSpeed;
}

void Animator::SetAnimationController(AnimationController* _animController)
{
	m_pAnimController = _animController;

	if (m_pAnimController != nullptr)
	{
		Animation* defaultAnim = m_pAnimController->GetDefaultAnimationFromSet();

		if (defaultAnim != nullptr)
			m_aInterpolators[m_uiCurrInterpolator].SetAnimation(defaultAnim);
	}
}

void Animator::SetSpeed(float _speed)
{
	m_fSpeed = _speed;
}

void Animator::ResetSpeed()
{
	m_fSpeed = 1.0f;
}

bool Animator::IsBlending() const
{
	return m_fElapsedBlendTime > 0.0f;
}

bool Animator::IsFinished()
{
	return (m_aInterpolators[m_uiCurrInterpolator].GetTime() >= m_aInterpolators[m_uiCurrInterpolator].GetAnimation()->GetDuration());
}

void Animator::SnapTo(Animation* _animation)
{
	m_fBlendTime = 0.0f;
	m_fElapsedBlendTime = 0.0f;
	m_aInterpolators[m_uiCurrInterpolator].SetAnimation(_animation);
}

void Animator::TransitionTo(Animation* _animation, float _blendTime)
{
	unsigned int nextInterpolator = 0;

	// std::string name = _animation->m_sName;

	// If we don't currently have an animation
	if (m_aInterpolators[m_uiCurrInterpolator].GetAnimation() == nullptr || _blendTime <= 0.0f)
	{
		// Snap to this new animation
		SnapTo(_animation);
	}
	else
	{
		// Set next interpolator
		nextInterpolator = m_uiCurrInterpolator ^ 0x1;

		m_fBlendTime = _blendTime;
		m_fElapsedBlendTime = 0.0f;

		m_aInterpolators[nextInterpolator].SetAnimation(_animation);
	}
}

void Animator::Update()
{
	// If the animation controller was set, so was our current interpolator
	if (m_pAnimController != nullptr)
	{
		Animation* currAnim = m_aInterpolators[m_uiCurrInterpolator].GetAnimation();

		// Can we make a transition to another animation?
		Animation* nextAnim = nullptr;
		if (m_fBlendTime == 0.0f)
		{
			nextAnim = m_pAnimController->CheckForTransition(currAnim->m_sName);
		}

		// If we're already making this transition..
		//Animation* temp = m_aInterpolators[m_uiCurrInterpolator ^ 0x1].GetAnimation();
		//if (temp != nullptr && nextAnim != nullptr && temp->m_sName == nextAnim->m_sName)
		//{
		//	// don't transition.
		//	nextAnim = nullptr;
		//}
		// if we're blending animations, scrap the trasition
		//if (m_fBlendTime > 0.0f)
		//nextAnim = nullptr;

		// If there's a transition to make
		if (nextAnim != nullptr)
		{
			// Get the blending time for this transition
			float blendTime = m_pAnimController->GetBlendTimeFromSet(currAnim->m_sName, nextAnim->m_sName);

			// Set up to interpolate animations
			TransitionTo(nextAnim, blendTime);
		}

		// Update Interpolators (Creates final Pose for this frame)
		ProcessInterpoloators();

		m_pRenderer->m_vBoneOffsets = GetPose();
	}
}

void Animator::ProcessInterpoloators()
{
	m_vPose.resize(m_aInterpolators[0].GetPose().size());

	unsigned int nextInterpolator = m_uiCurrInterpolator ^ 0x1;

	// if we're blending
	if (m_fBlendTime > 0.0f)
	{
		// Update elapsed blend time
		m_fElapsedBlendTime += Time::DeltaTime;

		// If we're done blending
		if (m_fElapsedBlendTime >= m_fBlendTime)
		{
			// swap interpolators
			m_uiCurrInterpolator = nextInterpolator;
			nextInterpolator = m_uiCurrInterpolator ^ 0x1;

			// Clear blend time data
			m_fBlendTime = m_fElapsedBlendTime = 0.0f;
		}
	}

	// Add time to our current interpolator
	if (m_aInterpolators[m_uiCurrInterpolator].GetAnimation()->IsReversed() == true)
	{
		m_aInterpolators[m_uiCurrInterpolator].AddTime((Time::DeltaTime * m_fSpeed) * -1.0f);
	}
	else
	{
		m_aInterpolators[m_uiCurrInterpolator].AddTime(Time::DeltaTime * m_fSpeed);
	}

	// Update current interpolator pose
	m_aInterpolators[m_uiCurrInterpolator].ProcessAnimation(m_pRenderer->m_vBones);

	// If we're still blending
	if (m_fBlendTime > 0.0f)
	{
		// Update next interpolator pose
		m_aInterpolators[nextInterpolator].AddTime(Time::DeltaTime);
		m_aInterpolators[nextInterpolator].ProcessAnimation(m_pRenderer->m_vBones);

		// Blend the 2 animations
		float lerpRatio = m_fElapsedBlendTime / m_fBlendTime;
		size_t boneCount = m_vPose.size();
		for (size_t bone = 0; bone < boneCount; bone++)
		{
			Matrix4 currBone = m_aInterpolators[m_uiCurrInterpolator].GetPose()[bone];
			Matrix4 nextBone = m_aInterpolators[nextInterpolator].GetPose()[bone];

			//XMMATRIX mat1 = XMLoadFloat4x4(&currBone);
			//XMVECTOR trans1, rot1, scale1;
			//XMMatrixDecompose(&scale1, &rot1, &trans1, mat1);

			//XMMATRIX mat2 = XMLoadFloat4x4(&nextBone);
			//XMVECTOR trans2, rot2, scale2;
			//XMMatrixDecompose(&scale2, &rot2, &trans2, mat2);

			//XMVECTOR finalTrans = XMVectorLerp(trans1, trans2, lerpRatio);
			//XMVECTOR finalRot = XMQuaternionSlerp(rot1, rot2, lerpRatio);
			//XMVECTOR finalScale = XMVectorLerp(scale1, scale2, lerpRatio);

			//XMMATRIX finalMat = XMMatrixAffineTransformation(finalScale, XMVectorZero(), finalRot, finalTrans);

			//XMStoreFloat4x4(&m_vPose[bone], finalMat);

			// Linear Interpolation. Not Good. Use DirectX Math.
			m_vPose[bone] = (currBone * (1.0f - lerpRatio) + nextBone * lerpRatio);
		}
	}
	else // We're not blending
	{
		// Use the pose from our current interpolator
		m_vPose = m_aInterpolators[m_uiCurrInterpolator].GetPose();
	}
}

// TODO::DELETE
// void Animator::SetTime(float time) { m_fCurrentTime = time; }
// void Animator::SetReversed(bool reversed) { m_bReverse = reversed; }
//void Animator::OldUpdate()
//{
//	if (m_pAnimation != nullptr)
//	{
//		if (!m_pAnimation->IsLooping() && m_fCurrentTime >= m_pAnimation->GetDuration())
//			return;
//
//		m_fCurrentTime += Time::DeltaTime;
//
//		while (m_fCurrentTime < 0.0f)
//			m_fCurrentTime += m_pAnimation->GetDuration();
//		while (m_fCurrentTime > m_pAnimation->GetDuration())
//			m_fCurrentTime -= m_pAnimation->GetDuration();
//
//		if (m_pRenderer)
//			m_pAnimation->Update(m_pRenderer->m_vBones, m_pRenderer->m_vBoneOffsets, m_fCurrentTime);
//	}
//}

//void Animator::SetAnimation(Animation* anim)
//{
//	m_pAnimation = anim;
//}

//
// void Animator::Play()
// {
// 	if (m_pAnimation)
// 		SetTime(0.0f);
// }
//
// bool Animator::IsFinished() const
// {
// 	if (m_pAnimation)
// 	{
// 		if (m_pAnimation->IsLooping() || m_pAnimation->GetDuration() > m_fCurrentTime)
// 			return false;
// 		else
// 			return true;
// 	}
//
// 	return false;
// }
// bool Animator::IsPlaying() const { return m_bPlaying; }
// bool Animator::IsReversed() const { return m_bReverse; }
//
// TODO::END