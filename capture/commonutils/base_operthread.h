#pragma once
#include "base_queuethread.h"

class CBaseOperThread: public CBaseQueuethread
{
public:
	CBaseOperThread(void);
public:
	virtual ~CBaseOperThread(void);
public:
	bool Start( void* pParam );
	bool ReStart();
	virtual void StopPauseThread();
protected:
	virtual unsigned int Run();
	void MyPostMessage( HWND hWnd, UINT Msg,WPARAM wParam, LPARAM lParam );
private:
	static unsigned int __stdcall EntryPoint( void* pArg );
protected:
	void* m_pParam;
};
