//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "Assets\AnimationController.h"
#include "InfernoTimer.h"

#include "AssertHelper.h"

template<class type>
bool checkComparison(type _lhs, type _rhs, AnimationController::OPERATOR _op)
{
	bool ret = false;

	switch (_op)
	{
	case AnimationController::OPERATOR::LESS_THEN:
	{
		ret = _lhs < _rhs;
	}
	break;
	case AnimationController::OPERATOR::LESS_THEN_OR_EQUAL:
	{
		ret = _lhs <= _rhs;
	}
	break;
	case AnimationController::OPERATOR::EQUAL:
	{
		ret = (_lhs == _rhs);
	}
	break;
	case AnimationController::OPERATOR::GREATER_THEN_OR_EQUAL:
	{
		ret = _lhs >= _rhs;
	}
	break;
	case AnimationController::OPERATOR::GREATER_THEN:
	{
		ret = _lhs > _rhs;
	}
	break;
	case AnimationController::OPERATOR::NOT_EQUAL:
	{
		ret = _lhs != _rhs;
	}
	break;
	default:
		break;
	}

	return ret;
}

std::vector<AnimationController::TransitionBase>* AnimationController::GetTransitionInfos(std::string& _animName)
{
	std::vector<AnimationController::TransitionBase> temp;

	auto iter = m_vTransitionInfos.begin();
	while (iter != m_vTransitionInfos.end())
	{
		if (iter->first == _animName)
			return &iter->second;

		++iter;
	}

	return nullptr;
}

AnimationController::AnimationController()
{
	m_pAnimationSet = nullptr;
}

AnimationController::~AnimationController()
{
	for (size_t index = 0; index < m_vTransitionInfos.size(); index++)
	{
		for (size_t infos = 0; infos < m_vTransitionInfos[index].second.size(); infos++)
		{
			for (size_t condition = 0; condition < m_vTransitionInfos[index].second[infos].conditions.size(); condition++)
			{
				delete m_vTransitionInfos[index].second[infos].conditions[condition];
			}
		}

			m_vTransitionInfos[index].second.clear();
	}

	m_vTransitionInfos.clear();
}

AnimationController::AnimationController(const AnimationController& _rhs)
{
	*this = _rhs;
}

AnimationController& AnimationController::operator=(const AnimationController& _rhs)
{
	// Saftey check
	if (this == &_rhs)
		return *this;

	// Copy Members
	m_sName = _rhs.m_sName;

	// Copy Pointers
	m_pAnimationSet = _rhs.m_pAnimationSet;

	// Copy Map Containers
	m_mBools = _rhs.m_mBools;
	m_mInts = _rhs.m_mInts;
	m_mFloats = _rhs.m_mFloats;

	// Copy Transitions
	size_t transitionInfosCount = _rhs.m_vTransitionInfos.size();
	m_vTransitionInfos.reserve(transitionInfosCount);

	for (size_t transitionInfos = 0; transitionInfos < transitionInfosCount; transitionInfos++)
	{
		std::vector<TransitionBase> copyInfos;
		size_t transitionCount = _rhs.m_vTransitionInfos[transitionInfos].second.size();
		copyInfos.reserve(transitionCount);

		for (size_t transition = 0; transition < transitionCount; transition++)
		{
			TransitionBase copyBase;
			copyBase.toAnimations = _rhs.m_vTransitionInfos[transitionInfos].second[transition].toAnimations;

			size_t conditionCount = _rhs.m_vTransitionInfos[transitionInfos].second[transition].conditions.size();
			copyBase.conditions.reserve(conditionCount);

			for (size_t condition = 0; condition < conditionCount; condition++)
			{
				TransitionInfoBase* copyInfoBase = new TransitionInfoBase;
				copyInfoBase->eventName = _rhs.m_vTransitionInfos[transitionInfos].second[transition].conditions[condition]->eventName;
				copyInfoBase->op = _rhs.m_vTransitionInfos[transitionInfos].second[transition].conditions[condition]->op;
				copyInfoBase->type = _rhs.m_vTransitionInfos[transitionInfos].second[transition].conditions[condition]->type;

				copyBase.conditions.push_back(copyInfoBase);
			}

			copyInfos.push_back(copyBase);
		}

		std::string fromAnim = _rhs.m_vTransitionInfos[transitionInfos].first;
		m_vTransitionInfos.push_back(std::pair<std::string, std::vector<TransitionBase>>(fromAnim, copyInfos));
		//m_vTransitionInfos[transitionInfos] = _rhs.m_vTransitionInfos[transitionInfos];
	}

	return *this;
}

Animation* AnimationController::CheckForTransition(std::string _currAnim)
{
	std::vector<TransitionBase>* currTransitionInfo = GetTransitionInfos(_currAnim);

	if (currTransitionInfo == nullptr)
		return nullptr;

	for (size_t index = 0; index < currTransitionInfo->size(); index++)
	{
		TransitionBase tempInfo = (*currTransitionInfo)[index];
		bool comparison = false;

		size_t conditionCount = tempInfo.conditions.size();
		for (size_t index = 0; index < conditionCount; index++)
		{
			TransitionInfoBase* tempCondition = tempInfo.conditions[index];

			switch (tempCondition->type)
			{
			case BOOL:
			{
				std::unordered_map<std::string, bool>::const_iterator iter = m_mBools.find(tempCondition->eventName);
				bool rhs = (bool)(static_cast<TransitionInfo<bool>*>(tempCondition)->value);
				
				// THIS IS A HACK, UPCASTING IS BAD
				if (rhs != 0)
					rhs = true;

				comparison = checkComparison<bool>((bool)(iter->second), rhs, tempCondition->op);
				break;
			}
			case INT:
			{
				std::unordered_map<std::string, int>::const_iterator iter = m_mInts.find(tempCondition->eventName);
				comparison = checkComparison<int>(iter->second, static_cast<TransitionInfo<int>*>(tempCondition)->value, tempCondition->op);
				break;
			}
			case FLOAT:
			{
				std::unordered_map<std::string, float>::const_iterator iter = m_mFloats.find(tempCondition->eventName);
				comparison = checkComparison<float>(iter->second, static_cast<TransitionInfo<float>*>(tempCondition)->value, tempCondition->op);
				break;
			}
			default:
				break;
			}

			if (comparison == false)
				break;
		}

		if (comparison == true)
		{
			size_t toAnimCount = tempInfo.toAnimations.size();
			int randIndex = 0;

			if (toAnimCount > 1)
				randIndex = rand() % (int)(toAnimCount);

			std::string toAnim = tempInfo.toAnimations[randIndex];

			//PRINT((_currAnim + ", " + toAnim + '\n'));

			return GetAnimationFromSet(toAnim);
		}
	}

	return nullptr;
}


bool AnimationController::GetBool(std::string& _name) const
{
	std::unordered_map<std::string, bool>::const_iterator iter = m_mBools.find(_name);

	return iter->second; // This will force a break if the name given is wrong. This will help with debugging.
}

int AnimationController::GetInt(std::string& _name) const
{
	std::unordered_map<std::string, int>::const_iterator iter = m_mInts.find(_name);

	return iter->second; // This will force a break if the name given is wrong. This will help with debugging.
}

float AnimationController::GetFloat(std::string& _name) const
{
	std::unordered_map<std::string, float>::const_iterator iter = m_mFloats.find(_name);

	return iter->second; // This will force a break if the name given is wrong. This will help with debugging.
}

void AnimationController::SetBool(std::string& _name, bool _value)
{
	std::unordered_map<std::string, bool>::const_iterator iter = m_mBools.find(_name);

	if (iter != m_mBools.end())	// This prevents adding accidental wrong spellings
		m_mBools[_name] = _value;
}

void AnimationController::SetInt(std::string& _name, int _value)
{
	std::unordered_map<std::string, int>::const_iterator iter = m_mInts.find(_name);

	if (iter != m_mInts.end())	// This prevents adding accidental wrong spellings
		m_mInts[_name] = _value;
}

void AnimationController::SetFloat(std::string& _name, float _value)
{
	std::unordered_map<std::string, float>::const_iterator iter = m_mFloats.find(_name);

	if (iter != m_mFloats.end())	// This prevents adding accidental wrong spellings
		m_mFloats[_name] = _value;
}

Animation* AnimationController::GetAnimationFromSet(std::string& _animName)
{
	if (m_pAnimationSet != nullptr)
	{
		return m_pAnimationSet->GetAnimation(_animName);
	}

	return nullptr;
}

Animation* AnimationController::GetDefaultAnimationFromSet()
{
	if (m_pAnimationSet != nullptr)
	{
		return m_pAnimationSet->GetDefaultAnimation();
	}

	return nullptr;
}

const float AnimationController::GetBlendTimeFromSet(std::string& _fromAnimName, std::string& _toAnimName)
{
	if (m_pAnimationSet != nullptr)
	{
		return m_pAnimationSet->GetBlendTime(_fromAnimName, _toAnimName);
	}

	return 0.0f;
}

void AnimationController::GetAnimationDurations(std::vector<InfernoTimer>& _durations)
{
	size_t animCount = m_pAnimationSet->GetAnimations().size();

	_durations.resize(animCount);

	for (size_t anim = 0; anim < animCount; anim++)
	{
		float duration = (m_pAnimationSet->GetAnimation(anim))->GetDuration();
		_durations[(unsigned int)anim].SetTime(duration);
	}
}

void AnimationController::SetAnimationSet(AnimationSet* _animSet)
{
	m_pAnimationSet = _animSet;
}