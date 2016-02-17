/*  编写：王岁明  
	时间：2010-12-15
	功能：线程的封装类 内部实现事件通知机制 来判断线程是否正在执行
	      可在线程执行内部调用 IsThreadStop函数 来判断线程是否需要退出
		  外部可调用StopThread函数来通知线程执行体 结束线程运行
*/
#pragma once
#include "base_type.h"
#include "base_define.h"

class CBaseMsgthread
{
public:
	CBaseMsgthread(void);
	CBaseMsgthread( uint8 cExitWay );
public:
	~CBaseMsgthread(void);
public:
	bool CreateThread( void * psa, unsigned cbStatck, PTHREAD_START pfn, void * param, unsigned fdwCreate );
	bool Start( void* pParam );
	void ThreadPostMessage( unsigned int uMsg, WPARAM wParam = 0, LPARAM lParam = 0 );
	bool IsTheadStop( int nWaitTime = 0 );
	virtual void StopThread( bool bAsyn=false );
	void ExitThead( int nExitCode = -1 );
	bool TerminateThread( int nExitCode = -1 );
	int  GetExitCode();
	void *GetParam();
protected:
	void InitThread();
	virtual unsigned int Run(); 
private:
	static unsigned int __stdcall EntryPoint( void* pArg );
protected:
	DWORD  m_dwThreadID;
	HANDLE m_hThread;
	HANDLE m_hEvent;
	HANDLE m_hMsgEvent;
	void* m_pParam;
};