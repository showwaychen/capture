#include "stdafx.h"
#include "winthread.h"
#include <process.h>

namespace base {
WinThread::WinThread(const wchar_t* thread_name) {
	if (thread_name)
		m_name = thread_name;
	m_thread = NULL;
	m_stop_event = NULL;
	m_stop = FALSE;
	m_state = kNOTHING;
}

WinThread::~WinThread() {
	Stop();
}

bool WinThread::Start() {
	Stop();
	m_stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD thread_id = 0;
	m_thread = ::CreateThread(NULL, 0, ThreadProc, this, 0, &thread_id);

	if (m_thread)
		m_state = kNEW;
	else
		m_state = kNOTHING;

	return m_thread != NULL;
}

void WinThread::Stop() {
	if (!m_thread)
		return;

	m_stop = TRUE;
	SetEvent(m_stop_event);
	Join();
	CloseHandle(m_thread);
	CloseHandle(m_stop_event);
	m_stop = FALSE;
	m_state = kTERMINATED;
	m_thread = NULL;
	m_stop_event = NULL;
}

void WinThread::Join() {
	if (m_thread)
		WaitForSingleObject(m_thread, INFINITE);
}

void WinThread::Join(long millis) {
	if (m_thread)
		WaitForSingleObject(m_thread, millis);
}

HANDLE WinThread::GetCurrent() const {
	return m_thread;
}

unsigned long WinThread::SetAffinityMask(unsigned long mask) {
	return (unsigned long)::SetThreadAffinityMask(m_thread, mask);
}

bool WinThread::SetPriority(int priority) {
	return (SetThreadPriority(m_thread, priority) != 0);
}

int WinThread::GetPriority() const{
	return GetThreadPriority(m_thread);
}

void WinThread::SetName(const wchar_t* thread_name) {
	if (thread_name) 
		m_name = thread_name;
}

const wchar_t* WinThread::GetName() const {
	return m_name.c_str();
}

int WinThread::GetState() const {
	return m_state;	
}

bool WinThread::GetTimes(FileTime* create_t, FileTime* exit_t, FileTime* kernel_t, FileTime* user_t) {
	return ::GetThreadTimes(m_thread, (LPFILETIME)create_t, (LPFILETIME)exit_t, (LPFILETIME)kernel_t, (LPFILETIME)user_t) != 0;
}

bool WinThread::_Sleep(unsigned long timeout) {
	return (WaitForSingleObject(m_stop_event, timeout) == WAIT_OBJECT_0);
}

void WinThread::SetRunning(bool flag) {
	if (flag) {
		if (m_thread == GetCurrent() && m_state == kNEW)
			m_state = kRUNNING;
	} else {
		m_state = kTERMINATED;
	}
}

unsigned long WinThread::ThreadProc(void* param) {
	WinThread* _thread = (WinThread*)param;
	if (_thread) {
		_thread->SetRunning(true);
		_thread->Run();
		_thread->SetRunning(false);
		return 1;
	}
	return 0;
}

void WinThread::Run() {

}
}

