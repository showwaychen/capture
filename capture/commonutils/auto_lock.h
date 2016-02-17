#pragma once
#include "sync_object.h"



namespace base {


class AutoLock
{
public:
	explicit AutoLock(CSyncObject* lock):m_lock(NULL)
	{
		m_lock = lock;
		if (m_lock)
			m_lock->Lock();
	}
	~AutoLock()
	{
		if (m_lock)
			m_lock->Unlock();
	}
protected:
	CSyncObject *m_lock;

};

}