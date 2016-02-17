#ifndef __win_thread_h__
#define __win_thread_h__

#include <string>
#include <windows.h>

typedef struct _FileTime {
	unsigned long low_datetime;
	unsigned long hi_datetime;
} FileTime, *PFileTime, *LPFileTime;

namespace base {
class WinThread {
public:
	WinThread(const wchar_t* thread_name = NULL);
	virtual ~WinThread();
	enum ThreadState{
		kNOTHING = 0,
		kNEW,
		kRUNNING,
		kTERMINATED = kNOTHING
	};

	//method
	bool Start();
	void Stop();

	void Join();
	void Join(long millis);

	//property
	unsigned long GetId() const;
	HANDLE GetCurrent() const;
	unsigned long SetAffinityMask(unsigned long mask);
	bool SetPriority(int priority);
	int GetPriority() const;
	void SetName(const wchar_t* thread_name);
	const wchar_t* GetName() const;
	int GetState() const;
	bool GetTimes(FileTime* create_t, FileTime* exit_t, FileTime* kernel_t, FileTime* user_t);

protected:
	bool _Sleep(unsigned long timeout);
	void SetRunning(bool flag);
	inline bool IsStop() {return m_stop;}
	virtual void Run();
	static unsigned long __stdcall ThreadProc(void* param);

private:
	std::wstring m_name;
	int m_state;
	bool m_stop;
	HANDLE m_stop_event;
	HANDLE m_thread;
};
}


#endif