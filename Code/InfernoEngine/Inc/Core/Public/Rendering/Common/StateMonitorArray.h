#pragma once

template<class T, unsigned int N>
class StateMonitorArray
{
	T m_State[N];							// Holds an array of all possible states
	T m_InitialState;						// Holds the initial state to be monitored

	unsigned int m_uiStartSlot;				// Holds which slot is the start slot for this state
	unsigned int m_uiEndSlot;				// Holds which slot is the end slot for this state

	StateMonitorArray<T, N>* m_pPrevious;	// A pointer to the previous state that will be used for comparisons
	bool m_bNeedsUpdate;					// Holds if we need to update the current state

public:
	StateMonitorArray(T initialState);
	~StateMonitorArray();

	// IN: StateMonitorArray<T, N>* - state to set as our previous state
	// OUT: void
	//
	// Sets our previous state to compare against to the incoming state
	void SetPrevious(StateMonitorArray<T, N>* prev);
	// IN: unsigned int - slot to compare
	// OUT: bool - comparision between our current to previous state
	//
	// Compares our current state slot to our previous state slot
	bool SameAsPrevious(unsigned int slot) const;

	// IN: void
	// OUT: unsigned int - first slot
	//
	// Returns the first slot we are using
	unsigned int GetStartSlot();
	// IN: void
	// OUT: unsigned int - last slot
	//
	// Returns the last slot we are using
	unsigned int GetEndSlot();
	// IN: void
	// OUT: unsigned int - number of slots
	//
	// Returns the number of slots we are using
	unsigned int GetRange();

	// IN: T            - state change
	//     unsigned int - slot to change
	// OUT: void
	//
	// Updates the state at the requested slot
	void SetState(T state, unsigned int slot);
	// IN: unsigned int - slot to get
	// OUT: T - requested state
	//
	// Returns the state at the requested slot
	T GetState(unsigned int slot) const;
	// IN: void
	// OUT: T* - Pointer to the first slot state
	//
	// Returns the pointer to the first slot this state is using
	T* GetFirstSlot();
	// IN: void
	// OUT: T* - Pointer to the last slot state
	//
	// Returns the pointer to the last slot this state is using
	T* GetSlotLocation(unsigned int slot);
	// IN: void
	// OUT: T* - Pointer to the last slot state
	//
	// Returns the pointer to the last slot this state is using
	T* GetSlotLocation(unsigned int slot) const;

	
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

template<class T, unsigned int N>
StateMonitorArray<T, N>::StateMonitorArray(T initialState) : m_InitialState(initialState), m_uiStartSlot(0), m_uiEndSlot(0), m_pPrevious(nullptr)
{
	InitializeState();
	ResetTracking();
}
template<class T, unsigned int N>
StateMonitorArray<T, N>::~StateMonitorArray()
{}

// IN: StateMonitorArray<T, N>* - state to set as our previous state
// OUT: void
//
// Sets our previous state to compare against to the incoming state
template<class T, unsigned int N>
void StateMonitorArray<T, N>::SetPrevious(StateMonitorArray<T, N>* prev)
{
	m_pPrevious = prev;
}
// IN: unsigned int - slot to compare
// OUT: bool - comparision between our current to previous state
//
// Compares our current state slot to our previous state slot
template<class T, unsigned int N>
bool StateMonitorArray<T, N>::SameAsPrevious(unsigned int slot) const { return m_State[slot] == m_pPrevious->m_State[slot]; }

// IN: void
// OUT: unsigned int - first slot
//
// Returns the first slot we are using
template<class T, unsigned int N>
unsigned int StateMonitorArray<T, N>::GetStartSlot() { return m_uiStartSlot; }
// IN: void
// OUT: unsigned int - last slot
//
// Returns the last slot we are using
template<class T, unsigned int N>
unsigned int StateMonitorArray<T, N>::GetEndSlot() { return m_uiEndSlot; }
// IN: void
// OUT: unsigned int - number of slots
//
// Returns the number of slots we are using
template<class T, unsigned int N>
unsigned int StateMonitorArray<T, N>::GetRange() { return (m_uiEndSlot - m_uiStartSlot) + 1; }

// IN: T            - state change
//     unsigned int - slot to change
// OUT: void
//
// Updates the state at the requested slot
template<class T, unsigned int N>
void StateMonitorArray<T, N>::SetState(T state, unsigned int slot)
{
	m_State[slot] = state;

	// If our previous was not set, set our update bool to true and set our start and end slots
	// to the extremes to update everything
	if (m_pPrevious == nullptr)
	{
		m_bNeedsUpdate = true;
		m_uiStartSlot = 0;
		m_uiEndSlot = N - 1;
		return;
	}

	bool slotSame = SameAsPrevious(slot);

	if (!m_bNeedsUpdate && !slotSame)
	{
		m_uiStartSlot = m_uiEndSlot = slot;
		m_bNeedsUpdate = true;
	}

	if (m_bNeedsUpdate)
	{
		if (slot < m_uiStartSlot)
		{
			// If the slot is less then our start slot and is not the same as our previous state,
			// we will need to update our start slot
			if (!slotSame)
				m_uiStartSlot = slot;
		}

		if (m_uiEndSlot < slot)
		{
			// If the slot is greater then our end slot and is not the same as our previous state,
			// we will need to update our end slot
			if (!slotSame)
				m_uiEndSlot = slot;
		}

		if (m_uiStartSlot == slot)
		{
			if (slotSame)
			{
				// If our slot is the same as our start slot and happens to be the same as our previous state, we need
				// to try to find the next slot that is not the same. We will work our way up to the end slot or difference in
				// slots, which comes first.
				for (; m_uiStartSlot < m_uiEndSlot; m_uiStartSlot++)
				{
					if (!SameAsPrevious(m_uiStartSlot))
						break;
				}

				// If we start and end are equal and our end is the same as the previous state we can reset our tracking as
				// all the states are now the same.
				if (m_uiStartSlot == m_uiEndSlot && SameAsPrevious(m_uiEndSlot))
					ResetTracking();
			}
		}

		if (m_uiEndSlot == slot)
		{
			if (slotSame)
			{
				// If our slot is the same as our end slot and happens to be the same as our previous state, we need
				// to try to find the next slot that is not the same. We will work our way down to the start slot or difference in
				// slots, which comes first.
				for (; m_uiEndSlot < m_uiStartSlot; m_uiEndSlot--)
				{
					if (!SameAsPrevious(m_uiEndSlot))
						break;
				}

				// If we start and end are equal and our start is the same as the previous state we can reset our tracking as
				// all the states are now the same.
				if (m_uiStartSlot == m_uiEndSlot && SameAsPrevious(m_uiStartSlot))
					ResetTracking();
			}
		}
	}
}
// IN: unsigned int - slot to get
// OUT: T - requested state
//
// Returns the state at the requested slot
template<class T, unsigned int N>
T StateMonitorArray<T, N>::GetState(unsigned int slot) const { return m_State[slot]; }
// IN: void
// OUT: T* - Pointer to the first slot state
//
// Returns the pointer to the first slot this state is using
template<class T, unsigned int N>
T* StateMonitorArray<T, N>::GetFirstSlot() { return &m_State[m_uiStartSlot]; }
// IN: void
// OUT: T* - Pointer to the last slot state
//
// Returns the pointer to the last slot this state is using
template<class T, unsigned int N>
T* StateMonitorArray<T, N>::GetSlotLocation(unsigned int slot) { return &m_State[slot]; }
// IN: void
// OUT: T* - Pointer to the last slot state
//
// Returns the pointer to the last slot this state is using
template<class T, unsigned int N>
T* StateMonitorArray<T, N>::GetSlotLocation(unsigned int slot) const { return &m_State[slot]; }


// IN: void
// OUT: bool - state if we need to update this state
//
// Returns the status of if we need to update the state or not
template<class T, unsigned int N>
bool StateMonitorArray<T, N>::UpdateNeeded() const { return m_bNeedsUpdate; }
// IN: void
// OUT: void
//
// Sets the state back to it's initial state
template<class T, unsigned int N>
void StateMonitorArray<T, N>::InitializeState()
{
	for (unsigned int i = 0; i < N; i++)
		m_State[i] = m_InitialState;
}
// IN: void
// OUT: void
//
// Resets the update bool back to false
template<class T, unsigned int N>
void StateMonitorArray<T, N>::ResetTracking()
{
	m_uiStartSlot = 0;
	m_uiEndSlot = 0;
	m_bNeedsUpdate = false;
}