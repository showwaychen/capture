/*  ��д��������  
	ʱ�䣺2010-12-15
	���ܣ��̵߳ķ�װ�� �ڲ�ʵ���¼�֪ͨ���� ���ж��߳��Ƿ�����ִ��
	      �����߳�ִ���ڲ����� IsThreadStop���� ���ж��߳��Ƿ���Ҫ�˳�
		  �ⲿ�ɵ���StopThread������֪ͨ�߳�ִ���� �����߳�����
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