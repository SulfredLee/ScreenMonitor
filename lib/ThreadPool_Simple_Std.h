#pragma once
#include <thread>
#include <atomic>
#include <vector>

#include "ConditionRingQ_Std.h"

class JoinThreads
{
	std::vector<std::thread>& threads;
public:
	explicit JoinThreads(std::vector<std::thread>& threads_) :
		threads(threads_)
	{}
	~JoinThreads()
	{
		for (unsigned long i = 0; i<threads.size(); ++i)
		{
			if (threads[i].joinable())
				threads[i].join();
		}
	}
};

class CThreadPool_Simple_Std
{
private:
	std::atomic<bool> m_bDone;
	CConditionRingQ_Std<std::function<void()> > m_FunctionQueue;
	std::vector<std::thread> m_Threads;
	JoinThreads m_Joiner;

	void WorkerThread()
	{
		while (!m_bDone)
		{
			std::shared_ptr<std::function<void()> > task;
			if (m_FunctionQueue.get(task))
			{
				(*task)();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
public:
	CThreadPool_Simple_Std() :
		m_bDone(false), m_Joiner(m_Threads)
	{
		unsigned const int cnThreadCount = std::thread::hardware_concurrency() - 1;
		try
		{
			for (unsigned int i = 0; i < cnThreadCount; i++)
			{
				m_Threads.emplace_back(
					std::thread(&CThreadPool_Simple_Std::WorkerThread, this));
			}
		}
		catch (...)
		{
			m_bDone = true;
			throw;
		}
	}
	~CThreadPool_Simple_Std()
	{
		m_bDone = true;
		// make all thread wake up and join by m_Joiner
		// so that this class can be killed
		unsigned const int cnThreadCount = std::thread::hardware_concurrency();
		for (unsigned int i = 0; i < cnThreadCount; i++)
		{
			m_FunctionQueue.push(std::function<void()>([]{ return; }));
		}
	}

	template<typename FunctionType>
	void submit(FunctionType f)
	{
		m_FunctionQueue.push(std::function<void()>(f));
	}
};

