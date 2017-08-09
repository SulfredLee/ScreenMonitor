#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

template<typename T>
class CConditionRingQ_Std
{
private:
	std::mutex MutTail;
	std::mutex MutHead;
	std::mutex MutSection;
	std::condition_variable ConditionVar;
	std::vector<std::shared_ptr<T> > m_RingQueue;
	std::atomic<int> m_nIN;
	std::atomic<int> m_nOUT;
public:
	CConditionRingQ_Std()
	{
		m_RingQueue.resize(128);
		m_nIN = 0;
		m_nOUT = 0;
	}
	~CConditionRingQ_Std()
	{}

	bool push(T&& inData)
	{
		std::unique_lock<std::mutex> lk_Head(MutHead);
		int nNextIN = ((m_nIN + 1) & (128 - 1)); // mod operation
		if (nNextIN == m_nOUT) // if the next index of IN is index of OUT
		{
			return false;
		}
		m_RingQueue[m_nIN] = std::make_shared<T>(std::forward<T>(inData));
		m_nIN = nNextIN; // InterlockedCompareExchange((long*)&m_nIN, nNextIN, m_nIN);
		lk_Head.unlock();
		//inData = nullptr;
		ConditionVar.notify_one();
		return true;
	}

	bool get(std::shared_ptr<T>& outData)
	{
		std::unique_lock<std::mutex> lk_Tail(MutTail);
		ConditionVar.wait(lk_Tail, [&]{return !(m_nOUT == m_nIN); }); // if no more data in queue

		outData = m_RingQueue[m_nOUT];
		int nNextOUT = ((m_nOUT + 1) & (128 - 1)); // mod operation
		m_nOUT = nNextOUT; // InterlockedCompareExchange((long*)&m_nOUT, nNextOUT, m_nOUT);
		return true;
	}
};

