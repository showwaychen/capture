#include "StdAfx.h"
#include "base_msgthread.h"

CBaseMsgthread::CBaseMsgthread(void)
{
	m_hThread    = NULL;
	m_dwThreadID = 0;
	m_pParam = NULL;
	m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hMsgEvent = NULL;
}

CBaseMsgthread::~CBaseMsgthread(void)
{
	if( m_hEvent != NULL )
	{
		CloseHandle( m_hEvent );
		m_hEvent = NULL;
	}
	if( m_hMsgEvent != NULL )
	{
		CloseHandle( m_hMsgEvent );
		m_hMsgEvent = NULL;
	}
	TerminateThread( -1 );
}
bool CBaseMsgthread::CreateThread( void * psa, unsigned cbStatck, PTHREAD_START pfn, void * param, unsigned fdwCreate )
{
//	m_param.m_arg = param;
	if( m_hMsgEvent == NULL )
		m_hMsgEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	ResetEvent( m_hMsgEvent );
	ResetEvent( m_hEvent );
	m_hThread = chBEGINTHREADEX( psa, cbStatck, pfn, param, fdwCreate, &m_dwThreadID );
	if( m_hThread == NULL )
		return false;
	return true;
}
bool CBaseMsgthread::Start( void* pParam )
{
	if( m_hThread == NULL )
	{
		m_pParam = pParam;
		if( !CreateThread( NULL, 0, EntryPoint, this, 0/*CREATE_SUSPENDED*/ ))
		{
			return false;
		}
// 		if( ResumeThread( m_hThread ) == -1) 
// 		{
// 			return false;
// 		}
// 		else
			return true;
	}
	return false;
}
unsigned int __stdcall CBaseMsgthread::EntryPoint( void* pArg )
{
	CBaseMsgthread* pThis = (CBaseMsgthread*)pArg;
	pThis->InitThread();
	unsigned int nRet = pThis->Run();
	return nRet;
}
void CBaseMsgthread::InitThread()
{
	MSG msg;
	::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ); //让线程创建消息队列
	SetEvent( m_hMsgEvent );
}
unsigned int CBaseMsgthread::Run()
{
	/*
	MSG msg;
	while( GetMessage(&msg, NULL, 0, 0) )
	{
		DispatchMessage(&msg); 
	}
	*/
	return 0;
}
void CBaseMsgthread::ThreadPostMessage( unsigned int uMsg, WPARAM wParam/* = 0*/, LPARAM lParam/* = 0*/ )
{
	if( m_hMsgEvent != NULL )
	{
		WaitForSingleObject( m_hMsgEvent, INFINITE );
		CloseHandle( m_hMsgEvent );
		m_hMsgEvent = NULL;
	}
	
	::PostThreadMessage( m_dwThreadID, uMsg, wParam, lParam );
}
bool CBaseMsgthread::IsTheadStop( int nTime /*=1000*/ )
{
	DWORD dwRet = WaitForSingleObject( m_hEvent, nTime );
	if( dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED || dwRet == WAIT_FAILED )
		return true;
	else
		return false;
}

void CBaseMsgthread::ExitThead( int nExitCode )
{
	::ExitThread( nExitCode );
	SetEvent( m_hEvent );
}
bool CBaseMsgthread::TerminateThread( int nExitCode )
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

void CBaseMsgthread::StopThread( bool bAsyn )
{
	SetEvent( m_hEvent );
	if( m_hThread != NULL )
	{
		::PostThreadMessage( m_dwThreadID, WM_QUIT, 0, 0 );
		if( !bAsyn )
			WaitForSingleObject( m_hThread, INFINITE );
		CloseHandle( m_hThread );
    	m_hThread = NULL;
	}
}
int  CBaseMsgthread::GetExitCode()
{
	DWORD dwRetCode = 0;
	::GetExitCodeThread( m_hThread, &dwRetCode );
	return dwRetCode;
}
void *CBaseMsgthread::GetParam()
{
	return m_pParam;
}
