#include "StdAfx.h"
#include "base_queuethread.h"

CBaseQueuethread::CBaseQueuethread(void)
{
	m_bIsExit = false;
	m_hThread    = NULL;
	m_dwThreadID = 0;
	m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
}

CBaseQueuethread::~CBaseQueuethread(void)
{
	if( m_hEvent != NULL )
	{
		CloseHandle( m_hEvent );
		m_hEvent = NULL;
	}
	ClearSemaphore();
	TerminateThread( -1 );
}
bool CBaseQueuethread::CreateThread( void * psa, unsigned cbStatck, PTHREAD_START pfn, void * param, unsigned fdwCreate )
{
//	m_param.m_arg = param;
	ResetEvent( m_hEvent );
	m_hThread = chBEGINTHREADEX( psa, cbStatck, pfn, param, fdwCreate, &m_dwThreadID );
	if( m_hThread == NULL )
		return false;
	return true;
}
bool CBaseQueuethread::IsTheadStop( int nTime /*=1000*/ )
{
	DWORD dwRet = WaitForSingleObject( m_hEvent, nTime );
	if( dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED || dwRet == WAIT_FAILED )
		return true;
	else
		return false;
}

void CBaseQueuethread::ExitThead( int nExitCode )
{
	::ExitThread( nExitCode );
	SetEvent( m_hEvent );
}
bool CBaseQueuethread::TerminateThread( int nExitCode )
{
	if( m_hThread != NULL )
	{
		if( ::TerminateThread( m_hThread, nExitCode )==TRUE )
		{
			SetEvent( m_hEvent );
			return true;
		}
		else
			return false;
	}
	else
		return true;
}

void CBaseQueuethread::StopThread( bool bAsyn )
{
	Exit();
	SetEvent( m_hEvent );
	if( m_hThread != NULL )
	{
		if( !bAsyn )
			WaitForSingleObject( m_hThread, INFINITE );
		CloseHandle( m_hThread );
    	m_hThread = NULL;
	}
}
int  CBaseQueuethread::GetExitCode()
{
	DWORD dwRetCode = 0;
	::GetExitCodeThread( m_hThread, &dwRetCode );
	return dwRetCode;
}

bool CBaseQueuethread::PushMsg( UINT message, WPARAM wParam, LPARAM lParam )
{
	MSG msg;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	m_msgQueue.push( msg );
	return true;
}
void CBaseQueuethread::wait_and_pop_lastMsg(MSG& msg)
{
	//m_msgQueue.wait_and_pop_last( msg );
	m_msgQueue.wait_and_pop( msg );
}
int CBaseQueuethread::GetSemaphoreNum()
{
	return m_msgQueue.get_semaphore_num();
}
void CBaseQueuethread::ClearSemaphore()
{
	MSG msg;
	while( m_msgQueue.try_pop( msg ) );
}
void CBaseQueuethread::Exit()
{
	m_bIsExit = true;
	PushMsg( WM_QUIT, 0, 0 );
}