/*
	CSyncObject
	author:	souther
	desc  : 封装了MFC 形式的同步对象 具体使用参考MSDN
	1.0: 8/7/2010
*/
#include "stdafx.h"
#include "sync_object.h"


//#ifndef __AFX_H__
namespace base {

CSyncObject::operator HANDLE() const
{
	return m_hObject;
}

CSyncObject::CSyncObject(LPCTSTR pstrName) 
{ 
	m_hObject = NULL; 
} 

CSyncObject::~CSyncObject() 
{ 
	if (m_hObject != NULL) 
	{ 
		::CloseHandle(m_hObject); 
		m_hObject = NULL; 
	} 
}

BOOL CSyncObject::Lock(DWORD dwTimeout) 
{ 
	DWORD dwRet = ::WaitForSingleObject(m_hObject, dwTimeout); 
	if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED) 
		return TRUE; 
	
	return FALSE;
} // Semaphore与Mutex抽象一样

/////////////////////////////////////////////////////////////////////////////
// CMutex
CMutex::CMutex( BOOL bInitiallyOwn, LPCTSTR pstrName,
			    LPSECURITY_ATTRIBUTES lpsaAttribute /* = NULL */) : CSyncObject(pstrName)
{ 
	m_hObject = ::CreateMutex( lpsaAttribute, bInitiallyOwn, pstrName ); 
	if ( m_hObject == NULL )
		throw( std::runtime_error(" CreateMutex exception") );
}

CMutex::~CMutex() 
{

} 

BOOL CMutex::Unlock() 
{
	return ::ReleaseMutex(m_hObject); 
}

////////////////////////////////////////////////////////////////////////////
// CSemaphore

CSemaphore::CSemaphore(LONG lInitialCount, LONG lMaxCount, LPCTSTR pstrName, LPSECURITY_ATTRIBUTES lpsaAttributes) : CSyncObject(pstrName) 
{
	//std::assert( lMaxCount > 0 );
	//std::assert( lInitialCount <= lMaxCount ); 
	m_hObject = ::CreateSemaphore( lpsaAttributes, lInitialCount, lMaxCount, pstrName ); 
	if ( m_hObject == NULL )
		throw( std::runtime_error(" CreateSemaphore exception") );
}

CSemaphore::~CSemaphore() 
{ 
} 

BOOL CSemaphore::Unlock()
{
	return ::ReleaseSemaphore(m_hObject, 1, NULL ); 
}

BOOL CSemaphore::Unlock(LONG lCount, LPLONG lpPrevCount /* =NULL */)
{ 
	return ::ReleaseSemaphore(m_hObject, lCount, lpPrevCount); 
}

/////////////////////////////////////////////////////////////////////////////
// CEvent

CEvent::CEvent(BOOL bInitiallyOwn, BOOL bManualReset, LPCTSTR pstrName, LPSECURITY_ATTRIBUTES lpsaAttribute) : CSyncObject(pstrName) 
{ 
	m_hObject = ::CreateEvent( lpsaAttribute, bManualReset, bInitiallyOwn, pstrName ); 
	if ( m_hObject == NULL ) 
		throw( std::runtime_error(" CreateEvent exception") );
}

CEvent::~CEvent()
{

}

BOOL CEvent::SetEvent()
{
	//ASSERT(m_hObject != NULL); 
	return ::SetEvent(m_hObject);
}

BOOL CEvent::PulseEvent()
{ 
	//ASSERT(m_hObject != NULL); 
	return ::PulseEvent(m_hObject);
} 

BOOL CEvent::ResetEvent()
{ 
	//ASSERT(m_hObject != NULL); 
	return ::ResetEvent(m_hObject); 
} 

BOOL CEvent::Unlock()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCriticalSection

CCriticalSection::CCriticalSection() : CSyncObject(NULL) 
{ 
	BOOL bSuccess; 
	bSuccess = Init();
	if ( !bSuccess ) 
		throw( std::bad_alloc(" CriticalSection exception") );
}

CCriticalSection::operator CRITICAL_SECTION*()
{ 
	return (CRITICAL_SECTION*)&m_sect; 
}

BOOL CCriticalSection::TryLock()
{
	return ::TryEnterCriticalSection( &m_sect );
}

BOOL CCriticalSection::Unlock()
{ 
	::LeaveCriticalSection( &m_sect ); 
	return TRUE; 
} 

BOOL CCriticalSection::Lock() 
{
	__try
	{ 
		::EnterCriticalSection(&m_sect); 
	}
	__except(STATUS_NO_MEMORY == ::GetExceptionCode()) 
	{ 
		//throw( std::runtime_error(" EnterCriticalSection exception") ); }
	}

	return TRUE; 
} 

BOOL CCriticalSection::Lock(DWORD dwTimeout)
{ 
	//ASSERT(dwTimeout == INFINITE); 
	return Lock(); 
}

CCriticalSection::~CCriticalSection()
{
	::DeleteCriticalSection(&m_sect);

}

BOOL CCriticalSection::Init()
{
	__try
	{
		::InitializeCriticalSection(&m_sect);
	}
	__except( STATUS_NO_MEMORY == GetExceptionCode()) 
	{
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSingleLock

CSingleLock::CSingleLock(CSyncObject* pObject, BOOL bInitialLock/* = FALSE*/) 
{ 
	//ASSERT(pObject != NULL); 
	//ASSERT(pObject->IsKindOf(RUNTIME_CLASS(CSyncObject))); 
	if ( pObject == NULL ) 
		throw( std::invalid_argument(" CSingleLock pObject == NULL ") );

	m_pObject = pObject; 
	m_hObject = pObject->m_hObject; 
	m_bAcquired = FALSE; 
	if (bInitialLock) 
		Lock(); 
} 

BOOL CSingleLock::Lock(DWORD dwTimeOut /* = INFINITE */) 
{ 
	//ASSERT(m_pObject != NULL || m_hObject != NULL); 
	//ASSERT(!m_bAcquired); 
	m_bAcquired = m_pObject->Lock(dwTimeOut); 
	return m_bAcquired; 
} 

BOOL CSingleLock::Unlock() 
{ 
	//ASSERT(m_pObject != NULL); 
	if ( m_bAcquired ) 
		m_bAcquired = !m_pObject->Unlock();
	return !m_bAcquired;
}

BOOL CSingleLock::Unlock(LONG lCount, LPLONG lpPrevCount /* = NULL */) 
{ 
	//ASSERT(m_pObject != NULL); 
	if (m_bAcquired) 
		m_bAcquired = !m_pObject->Unlock(lCount, lpPrevCount);
	return !m_bAcquired;
}

BOOL CSingleLock::IsLocked()
{ 
	return m_bAcquired; 
}

}

//#endif // __AFX_H__