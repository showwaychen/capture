#include "StdAfx.h"
#include "base_operthread.h"
#include "base_log.h"


CBaseOperThread::CBaseOperThread(void)
:m_pParam(NULL)
{
}

CBaseOperThread::~CBaseOperThread(void)
{
}

bool CBaseOperThread::Start( void* pParam )
{
	if( m_hThread == NULL )
	{
		m_pParam = pParam;
		if( !CreateThread( NULL, 0, EntryPoint, this, CREATE_SUSPENDED ))
		{
			return false;
		}
		if( ResumeThread( m_hThread ) == -1) 
		{
			return false;
		}
		else
			return true;
	}
	return false;
}
bool CBaseOperThread::ReStart()
{
	if( m_hThread == NULL )
	{
		if( !CreateThread( NULL, 0, EntryPoint, this, CREATE_SUSPENDED ))
		{
			return false;
		}
		if( ResumeThread( m_hThread ) == -1) 
		{
			return false;
		}
		else
		{
			ClearSemaphore();
			m_bIsExit = false;
			return true;
		}
	}
	return false;
}
void CBaseOperThread::StopPauseThread()
{

}
unsigned int __stdcall CBaseOperThread::EntryPoint( void* pArg )
{
	CBaseOperThread* pThis = (CBaseOperThread*)pArg;
	unsigned int nRet = pThis->Run();
	return nRet;
}

unsigned int CBaseOperThread::Run()
{
	return 0;
}
void CBaseOperThread::MyPostMessage( HWND hWnd, UINT Msg,WPARAM wParam, LPARAM lParam )
{
	if( hWnd != NULL )
	{
		if( ::PostMessage( hWnd, Msg, (WPARAM)wParam, (LPARAM)lParam ) == 0 )
		{
			_ERROR("BaseOperThread postmessage(%d) failed(%d)\r\n", Msg, GetLastError());
		}
	}
}
