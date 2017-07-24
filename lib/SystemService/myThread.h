#pragma once
#include <boost/thread.hpp>

class myThread
{
public:
	boost::thread * m_t;
private:
	bool m_bThreadExit;
	boost::mutex m_mtx;
public:
	myThread();
	~myThread();

	void Start();
	virtual void ThreadMain() = 0;
	void EndThread();
	bool IsEndThread();
};

