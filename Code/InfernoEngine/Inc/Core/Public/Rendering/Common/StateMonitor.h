#pragma once

template<class T>
class StateMonitor
{
private:
	T m_State;						// Holds the state to monitor
	T m_InitialState;				// Holds the initial state of the monitored state

	StateMonitor<T>* m_pPrevious;	// A pointer to the previous state that will be used for comparisons
	bool m_bNeedsUpdate;			// Holds if we need to update the current state

public:
	StateMonitor(T initialState);
	~StateMonitor();

	// IN: StateMonitor<T>* - state pointer to track as previous state
	// OUT: void
	//
	// Sets our state to treat as the previous state for monitoring change
	void SetPrevious(StateMonitor<T>* prev);
	// IN: void
	// OUT: bool
	//
	// Returns the comparision between this state and the previous state
	bool SameAsPrevious() const;

	// IN: T - state to set to
	// OUT: void
	//
	// Sets the state to the incoming state and compares the state to the previous state
	void SetState(T state);
	// IN: void
	// OUT: T - returned state
	//
	// Returns the current state
	T GetState() const;

	// IN: void
	// OUT: bool - state if we need to update this state
	//
	// Returns the status of if we need to update the state or not
	bool UpdateNeeded() const;
	// IN: void
	// OUT: void
	//
	// Sets the state back to it's initial state
	void InitializeState();
	// IN: void
	// OUT: void
	//
	// Resets the update bool back to false
	void ResetTracking();
};

template<class T>
StateMonitor<T>::StateMonitor(T initialState) : m_InitialState(initialState), m_State(initialState), m_bNeedsUpdate(false), m_pPrevious(nullptr)
{}
template<class T>
StateMonitor<T>::~StateMonitor()
{}

// IN: StateMonitor<T>* - state pointer to track as previous state
// OUT: void
//
// Sets our state to treat as the previous state for monitoring change
template<class T>
void StateMonitor<T>::SetPrevious(StateMonitor<T>* prev) { m_pPrevious = prev; }
// IN: void
// OUT: bool
//
// Returns the comparision between this state and the previous state
template<class T>
bool StateMonitor<T>::SameAsPrevious() const { return m_State == m_pPrevious->m_State; }

// IN: T - state to set to
// OUT: void
//
// Sets the state to the incoming state and compares the state to the previous state
template<class T>
void StateMonitor<T>::SetState(T state)
{
	m_State = state;

	if (m_pPrevious == nullptr)
	{
		m_bNeedsUpdate = true;

		return;
	}

	m_bNeedsUpdate = !SameAsPrevious();
}
// IN: void
// OUT: T - returned state
//
// Returns the current state
template<class T>
T StateMonitor<T>::GetState() const { return m_State; }

// IN: void
// OUT: bool - state if we need to update this state
//
// Returns the status of if we need to update the state or not
template<class T>
bool StateMonitor<T>::UpdateNeeded() const { return m_bNeedsUpdate; }
// IN: void
// OUT: void
//
// Sets the state back to it's initial state
template<class T>
void StateMonitor<T>::InitializeState() { SetState(m_InitialState); }
// IN: void
// OUT: void
//
// Resets the update bool back to false
template<class T>
void StateMonitor<T>::ResetTracking() { m_bNeedsUpdate = false; }