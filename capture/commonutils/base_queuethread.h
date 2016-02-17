/*  ��д��������  
	ʱ�䣺2010-12-15
	���ܣ��̵߳ķ�װ�� �ڲ�ʵ���¼�֪ͨ���� ���ж��߳��Ƿ�����ִ��
	      �����߳�ִ���ڲ����� IsThreadStop���� ���ж��߳��Ƿ���Ҫ�˳�
		  �ⲿ�ɵ���StopThread������֪ͨ�߳�ִ���� �����߳�����
*/
#pragma once
#include "base_type.h"
#include "base_define.h"
#include "base_threadQueue.h"

class CBaseQueuethread	
{
public:
	CBaseQueuethread(void);
	CBaseQueuethread( uint8 cExitWay );
public:
	~CBaseQueuethread(void);
public:
	bool CreateThread( void * psa, unsigned cbStatck, PTHREAD_START pfn, void * param, unsigned fdwCreate );
	bool IsTheadStop( int nWaitTime = 0 );
	void StopThread( bool bAsyn=false );
	void ExitThead( int nExitCode = -1 );
	bool TerminateThread( int nExitCode = -1 );
	int  GetExitCode();

	bool PushMsg( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	void wait_and_pop_lastMsg(MSG& msg);
	int GetSemaphoreNum();
	void ClearSemaphore();
	void Exit();
	bool IsExit() { return m_bIsExit; };
protected:
	DWORD  m_dwThreadID;
	HANDLE m_hThread;
	HANDLE m_hEvent;


	CBase_threadQueue<MSG> m_msgQueue;
	bool m_bIsExit;
};