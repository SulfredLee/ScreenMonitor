#pragma once
#include <vector>
#include <memory>

template<typename T>
class CConditionRingQ_Win
{
private:
	CRITICAL_SECTION CritHead;
	CRITICAL_SECTION CritTail;
	CRITICAL_SECTION CritSection;
	CONDITION_VARIABLE ConditionVar;
	std::vector<std::shared_ptr<T> > m_RingQueue;
	int m_nIN;
	int m_nOUT;
public:
	CConditionRingQ_Win()
	{
		// initial lock
		InitializeCriticalSection(&CritSection);
		InitializeCriticalSection(&CritHead);
		InitializeCriticalSection(&CritTail);
		InitializeConditionVariable(&ConditionVar);

		m_RingQueue.resize(128);
		m_nIN = 0;
		m_nOUT = 0;
	}
	~CConditionRingQ_Win()
	{
		// delete lock
		DeleteCriticalSection(&CritSection);
		DeleteCriticalSection(&CritHead);
		DeleteCriticalSection(&CritTail);
	}

	bool push(T&& inData)
	{
		EnterCriticalSection(&CritHead);
		int nNextIN = ((m_nIN + 1) & (128 - 1)); // mod operation
		if (nNextIN == m_nOUT) // if the next index of IN is index of OUT
		{
			LeaveCriticalSection(&CritHead);
			return false;
		}
		m_RingQueue[m_nIN] = std::make_shared<T>(std::forward(inData));
		InterlockedCompareExchange((long*)&m_nIN, nNextIN, m_nIN); //m_nIN = nNextIN;
		LeaveCriticalSection(&CritHead);
		//inData = nullptr;
		WakeConditionVariable(&ConditionVar);
		return true;
	}

	bool get(std::shared_ptr<T>& outData)
	{
		EnterCriticalSection(&CritTail);
		while (m_nOUT == m_nIN) // if no more data in queue
		{
			SleepConditionVariableCS(&ConditionVar, &CritTail, INFINITE);
			//return false;
		}
		outData = m_RingQueue[m_nOUT];
		int nNextOUT = ((m_nOUT + 1) & (128 - 1)); // mod operation
		InterlockedCompareExchange((long*)&m_nOUT, nNextOUT, m_nOUT); // m_nOUT = nNextOUT;
		LeaveCriticalSection(&CritTail);
		return true;
	}
};

