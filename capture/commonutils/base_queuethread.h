/*  编写：王岁明  
	时间：2010-12-15
	功能：线程的封装类 内部实现事件通知机制 来判断线程是否正在执行
	      可在线程执行内部调用 IsThreadStop函数 来判断线程是否需要退出
		  外部可调用StopThread函数来通知线程执行体 结束线程运行
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