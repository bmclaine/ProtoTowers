//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "IComponent.h"

#include "AnimInterpolator.h"

class AnimationController;
class SkinnedMeshRenderer;
class GameObject;
class Transform;

class Animator : public IComponent
{
private:
	// TODO::Delete
	// Animation* m_pAnimation;
	// float m_fCurrentTime;
	// TODO::END

	SkinnedMeshRenderer* m_pRenderer;
	AnimationController* m_pAnimController;

	AnimInterpolator m_aInterpolators[2];
	std::vector<Matrix4> m_vPose;

	unsigned int m_uiCurrInterpolator;

	float m_fSpeed;

	float m_fBlendTime;
	float m_fElapsedBlendTime;

	void ProcessInterpoloators();

	virtual ~Animator() override;

	virtual IComponent& operator=(IComponent& _rhs) override;
	Animator& operator=(const Animator& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	// TODO::DELETE
	//void SetAnimation(Animation* _animation);
	// TODO::END

	Animator(GameObject* const gameObject, Transform* const transform);

	std::string& GetCurrentAnimation();
	AnimationController* GetAnimationController() const;
	std::vector<Matrix4>& GetPose();
	float GetSpeed();

	void SetAnimationController(AnimationController* _animController);
	void SetSpeed(float _speed);
	void ResetSpeed();

	bool IsBlending() const; 
	bool IsFinished();
	// bool IsPlaying() const;

	void SnapTo(Animation* _animation);
	void TransitionTo(Animation* _animation, float _blendTime);

	void Update();
	void OldUpdate();

	virtual void Init() override;
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
};