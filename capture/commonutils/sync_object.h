#ifndef __syncobject_h__
#define __syncobject_h__

#define LOCK_CS(p)		::EnterCriticalSection(p)	
#define UNLOCK_CS(p)	::LeaveCriticalSection(p)
#define INIT_CS(p)		::InitializeCriticalSection(p)
#define DELETE_CS(p)	::DeleteCriticalSection(p)					
#define TYPE_CS			CRITICAL_SECTION 

#include <windows.h>
#include <stdexcept>

/////////////////////////////////////////////////////////////////////////////
// Basic synchronization object


namespace base {


class CSyncObject
{
// Constructor
public:
	explicit CSyncObject(LPCTSTR pstrName);

// Attributes
public:
	operator HANDLE() const;
	HANDLE  m_hObject;

// Operations
	virtual BOOL Lock(DWORD dwTimeout = INFINITE);
	virtual BOOL Unlock() = 0;
	virtual BOOL Unlock(LONG /* lCount */, LPLONG /* lpPrevCount=NULL */)
		{ return TRUE; }

// Implementation
public:
	virtual ~CSyncObject();
	friend class CSingleLock;
	friend class CMultiLock;
};

////////////////////////////////////////////////////////////////////////////
// CSemaphore

class CSemaphore : public CSyncObject
{
// Constructor
public:
	/* explicit */ CSemaphore(LONG lInitialCount = 1, 
							  LONG lMaxCount = 1,
							  LPCTSTR pstrName=NULL, 
							  LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);

// Implementation
public:
	virtual ~CSemaphore();
	virtual BOOL Unlock() ;
	virtual BOOL Unlock(LONG lCount, LPLONG lprevCount = NULL);
};

/////////////////////////////////////////////////////////////////////////////
// CMutex

class CMutex : public CSyncObject
{
// Constructor
public:
	/* explicit */ CMutex(BOOL bInitiallyOwn = FALSE,
						  LPCTSTR lpszName = NULL,
						  LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);

// Implementation
public:
	virtual ~CMutex();
	BOOL Unlock();
};

/////////////////////////////////////////////////////////////////////////////
// CEvent

class CEvent : public CSyncObject
{
private:
   using CSyncObject::Unlock;

// Constructor
public:
	/* explicit */ CEvent(BOOL bInitiallyOwn = FALSE, 
						  BOOL bManualReset = FALSE,
						  LPCTSTR lpszNAme = NULL, 
						  LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);

// Operations
public:
	BOOL SetEvent();
	BOOL PulseEvent();
	BOOL ResetEvent();
	BOOL Unlock();

// Implementation
public:
	virtual ~CEvent();
};

/////////////////////////////////////////////////////////////////////////////
// CCriticalSection

class CCriticalSection : public CSyncObject
{
private:
   using CSyncObject::Unlock;

// Constructor
public:
	CCriticalSection();

// Attributes
public:
	operator CRITICAL_SECTION*();
	CRITICAL_SECTION m_sect;

// Operations
public:
	BOOL TryLock();
	BOOL Unlock();
	BOOL Lock();
	BOOL Lock(DWORD dwTimeout);

// Implementation
public:
	virtual ~CCriticalSection();

private:
	BOOL Init();
};

/////////////////////////////////////////////////////////////////////////////
// CSingleLock

class CSingleLock
{
// Constructors
public:
	explicit CSingleLock(CSyncObject* pObject, BOOL bInitialLock = FALSE);

// Operations
public:
	BOOL Lock(DWORD dwTimeOut = INFINITE);
	BOOL Unlock();
	BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL);
	BOOL IsLocked();

// Implementation
public:
	~CSingleLock();

protected:
	CSyncObject* m_pObject;
	HANDLE  m_hObject;
	BOOL    m_bAcquired;
};

/////////////////////////////////////////////////////////////////////////////
// CMultiLock

class CMultiLock
{
// Constructor
public:
	CMultiLock(CSyncObject* ppObjects[], DWORD dwCount, BOOL bInitialLock = FALSE);

// Operations
public:
	DWORD Lock(DWORD dwTimeOut = INFINITE, BOOL bWaitForAll = TRUE,
		DWORD dwWakeMask = 0);
	BOOL Unlock();
	BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL);
	BOOL IsLocked(DWORD dwItem);

// Implementation
public:
	~CMultiLock();

protected:
	HANDLE  m_hPreallocated[8];
	BOOL    m_bPreallocated[8];

	CSyncObject* const * m_ppObjectArray;
	HANDLE* m_pHandleArray;
	BOOL*   m_bLockedArray;
	DWORD   m_dwCount;
};

} //namespace

#endif
