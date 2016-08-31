//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "Animation.h"
#include "AnimationSet.h"
#include "GlobalIncludes.h"

class InfernoTimer;

class AnimationController
{
public:
	enum DATA_TYPE { BOOL, INT, FLOAT };
	enum OPERATOR { LESS_THEN, LESS_THEN_OR_EQUAL, EQUAL, GREATER_THEN_OR_EQUAL, GREATER_THEN, NOT_EQUAL };

	std::string m_sName;

	AnimationController();
	~AnimationController();

	AnimationController(const AnimationController& _rhs);
	AnimationController& operator=(const AnimationController& _rhs);

	Animation* GetAnimationFromSet(std::string& _animName);
	Animation* GetDefaultAnimationFromSet();

	void GetAnimationDurations(std::vector<InfernoTimer>& _durations);
	const float GetBlendTimeFromSet(std::string& _fromAnimName, std::string& _toAnimName);

	void SetAnimationSet(AnimationSet* _animSet);

	Animation* CheckForTransition(std::string _currAnim);

	// Getters for the maps
	bool GetBool(std::string& _name) const;
	int GetInt(std::string& _name) const;
	float GetFloat(std::string& _name) const;

	// Setters for the maps
	void SetBool(std::string& _name, bool _value);
	void SetInt(std::string& _name, int _value);
	void SetFloat(std::string& _name, float _value);

	template<class type>
	bool AddTransition(std::string& _fromAnimName, DATA_TYPE _dataType, std::string& _eventName, type _value, OPERATOR _op, std::vector<std::string>& _toAnimNames);

	template<class type>
	bool AddAnyStateTransition(DATA_TYPE _dataType, std::string& _eventName, type _value, OPERATOR _op, std::vector<std::string>& _toAnimNames);

private:
	struct TransitionInfoBase
	{
		virtual ~TransitionInfoBase() {}

		DATA_TYPE type;
		std::string eventName;
		OPERATOR op;
	};

	template<class dataType>
	struct TransitionInfo : public TransitionInfoBase // Templated struct so we can store exactly what conditions we want
	{
		dataType value;

		bool operator==(const TransitionInfo& _rhs);
		bool operator!=(const TransitionInfo& _rhs);
	};

	struct TransitionBase
	{
		std::vector<std::string> toAnimations;
		std::vector<TransitionInfoBase*> conditions;
	};

	// The Animation Set for this controller
	AnimationSet* m_pAnimationSet;

	// All the storage options for events the player wants for animation transitioning
	std::unordered_map<std::string, bool> m_mBools;
	std::unordered_map<std::string, int> m_mInts;
	std::unordered_map<std::string, float> m_mFloats;

	// Stores our Animators and the list of transitions they can make
	std::vector<std::pair<std::string, std::vector<TransitionBase>>> m_vTransitionInfos;

	std::vector<TransitionBase>* GetTransitionInfos(std::string& _animName);
};

template<class type>
bool AnimationController::TransitionInfo<type>::operator==(const TransitionInfo& rhs)
{
	if (type != rhs.type || eventName != rhs.eventName || value != rhs.value || op != rhs.op)
		return false;

	return true;
}

template<class type>
bool AnimationController::TransitionInfo<type>::operator!=(const TransitionInfo& rhs)
{
	if (type != rhs.type || eventName != rhs.eventName || value != rhs.value ||
		op != rhs.op || toAnimator != rhs.toAnimator)
		return true;

	return false;
}

template<class type>
bool AnimationController::AddTransition(std::string& _fromAnimName, DATA_TYPE _dataType, std::string& _eventName, type _value, OPERATOR _op, std::vector<std::string>& _toAnimNames)
{
	TransitionInfo<type>* currTransition;
	TransitionInfo<type>* tempTransition = new TransitionInfo<type>;

	tempTransition->type = _dataType;
	tempTransition->eventName = _eventName;
	tempTransition->value = _value;
	tempTransition->op = _op;

	// Check to see if the fromAnimation is in the animation set
	if (m_pAnimationSet->CheckForAnimation(_fromAnimName) == false)
	{
		delete tempTransition;
		return false;
	}

	// Check to see if each toAnimation is in the animation set
	size_t toAnimCount = _toAnimNames.size();
	for (size_t toAnim = 0; toAnim < toAnimCount; toAnim++)
	{
		if (m_pAnimationSet->CheckForAnimation(_toAnimNames[toAnim]) == false || _fromAnimName == _toAnimNames[toAnim])
		{
			delete tempTransition;
			return false;
		}
	}

	// Check to see if fromAnim is already in the transitions list
	auto fromIter = m_vTransitionInfos.begin();
	while (fromIter != m_vTransitionInfos.end())
	{
		// If we found it, cool
		if (fromIter->first == _fromAnimName)
			break;

		++fromIter;
	}

	// If we didn't, add it
	if (fromIter == m_vTransitionInfos.end())
	{
		std::vector<TransitionBase> temp;
		m_vTransitionInfos.push_back(std::pair<std::string, std::vector<TransitionBase>>(_fromAnimName, temp));
		
		fromIter = m_vTransitionInfos.begin();
		while (fromIter != m_vTransitionInfos.end())
		{
			// If we found it, cool
			if (fromIter->first == _fromAnimName)
				break;

			++fromIter;
		}
	}

	// Check for a transition to toAnims
	unsigned int transitionIndex = 0;
	for (; transitionIndex < (unsigned int)fromIter->second.size(); transitionIndex++)
	{
		// If we find it, cool. We now have our transition index
		if (fromIter->second[transitionIndex].toAnimations == _toAnimNames)
			break;
	}

	// If the transition index was found
	if (transitionIndex != (unsigned int)fromIter->second.size())
	{
		// Check to see if the condition already exists
		for (size_t condition = 0; condition < fromIter->second[transitionIndex].conditions.size(); condition++)
		{
			TransitionInfoBase* tempBase = fromIter->second[transitionIndex].conditions[condition];

			if (typeid(tempBase) == typeid(TransitionInfo<type>))
				currTransition = static_cast<TransitionInfo<type>*>(tempBase);

			// if it does, forget about it
			if (currTransition == tempTransition)
			{
				delete tempTransition;
				return false;
			}
		}
	} // If the transition index was not found
	else
	{
		// Create a new Transition
		TransitionBase newBase;
		newBase.toAnimations = _toAnimNames;

		fromIter->second.push_back(newBase);
	}

	switch (_dataType)
	{
	case BOOL:
		m_mBools[_eventName] = false;
		break;
	case INT:
		m_mInts[_eventName] = 0;
		break;
	case FLOAT:
		m_mFloats[_eventName] = 0.0f;
		break;
	default:
		break;
	}

	// If everything goes well, we add the new transition
	fromIter->second[transitionIndex].conditions.push_back(tempTransition);
	return true;
}

template<class type>
bool AnimationController::AddAnyStateTransition(DATA_TYPE _dataType, std::string& _eventName, type _value, OPERATOR _op, std::vector<std::string>& _toAnimNames)
{
	size_t animCount = m_pAnimationSet->GetAnimations().size();
	for (size_t anim = 0; anim < animCount; anim++)
	{
		AddTransition(m_pAnimationSet->GetAnimation(anim)->m_sName, _dataType, _eventName, _value, _op, _toAnimNames);
	}

	return true;
}