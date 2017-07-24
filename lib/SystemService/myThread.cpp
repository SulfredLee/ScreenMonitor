#include "stdafx.h"
#include "myThread.h"


myThread::myThread()
{
	m_bThreadExit = false;
	m_t = NULL;
}


myThread::~myThread()
{
}

void myThread::Start()
{
	if (m_t != NULL)
		delete m_t;
	m_mtx.lock();
	m_bThreadExit = false;
	m_mtx.unlock();
	m_t = new boost::thread(boost::bind(&myThread::ThreadMain, this));
}

void myThread::EndThread()
{
	boost::lock_guard<boost::mutex> guard(m_mtx);
	m_bThreadExit = true;
}

bool myThread::IsEndThread()
{
	boost::lock_guard<boost::mutex> guard(m_mtx);
	return m_bThreadExit;
}