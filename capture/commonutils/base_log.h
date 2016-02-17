/************************************************************************/
/* 编写：王岁明  
   时间：2010-12-15
   功能：对日志文件写入的简单封装， 单实例模式，定义P2SP_DEBUG P2SP_LOG
         两种输出模式，
		      P2SP_DEBUG输出到默认输出流
			  P2SP_LOG  追加到文件尾部
	     目前只支持ANSI字符的输入

  
  ================日志模块重新改写==================  
   by souther
   2013-5-6
   1.使用了全局字符缓存,避免过多申请栈上的空间。
   2.支持文件名与代码行 输出
   3.之前的宏调用方式实在难用（天那，每次必须多打对括号）, 所以被抛弃，
   并扩充为3种输出日志方式。

   2013-11-06
   1.新增日志输出级别, 并支持通过配置文件过滤日志
   remark:
   日志过滤:在日志输出同级目录建立一个 log.ini 文件
   eg: 
   文件名 = 过滤码	参考 LOG_LEVEL_ 定义
   [filter]
   test.log = 3

USEAGE:
   Init:
   BASE_LOG_INIT("test.log");

   Use:
   _ERROR("This is _ERROR Log ERR ID=%d", 10);
   _WARNING("This is _WARNING Log");
   _TRACE("This is _TRACE Log");
   _INFO("This is _INFO Log");

   Unit:
   BASE_LOG_CLOSE();
*/
/************************************************************************/
#pragma once

//#include "base_common_file.h"
#include <Windows.h>
#include <string>

#ifndef _BASE_LOG_OUT
#define _BASE_LOG_OUT
#endif

#ifdef _BASE_LOG_OUT
#define BASE_LOG_INIT(X) \
	do{\
		base::BaseLog::Log()->OpenLog(X);\
	}while(0)

#define BASE_LOG_CLOSE() \
	do{\
		base::BaseLog::Unlog();\
	}while(0)
#else
#define BASE_LOG_INIT(X) \
	do{\
	}while(0)

#define BASE_LOG_CLOSE() \
	do{\
	}while(0)
#endif

#define LOG_NOFILTER

#define LOG_LEVEL_MASK			1
#define LOG_LEVEL_ALL			0x0000FFFF
#define LOG_LEVEL_ERROR			LOG_LEVEL_MASK
#define LOG_LEVEL_WARNING		(((int)LOG_LEVEL_MASK)<<1)	//2
#define LOG_LEVEL_TRACE			(((int)LOG_LEVEL_MASK)<<2)	//4
#define LOG_LEVEL_INFO			(((int)LOG_LEVEL_MASK)<<3)	//8


#define LOG_LEVEL_RELEASE		LOG_LEVEL_ERROR|LOG_LEVEL_WARNING	//3
#define LOG_LEVEL_DEBUG			LOG_LEVEL_ERROR|LOG_LEVEL_WARNING|LOG_LEVEL_TRACE|LOG_LEVEL_INFO  //15	

#ifdef _DEBUG
#define DEFAULT_LEVEL	LOG_LEVEL_DEBUG
#else
#define DEFAULT_LEVEL	LOG_LEVEL_DEBUG
#endif

#ifdef _BASE_LOG_OUT
	#define _ERROR(...) \
		base::BaseLog::Log()->OutFile(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

	#define _WARNING(...) \
		base::BaseLog::Log()->OutFile(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)

	#define _TRACE(...) \
		base::BaseLog::Log()->OutFile(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)

	#define _INFO(...) \
		base::BaseLog::Log()->OutFile(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)

	#define _DLOG(LEVEL, ...) \
		base::BaseLog::Log()->OutFile(LEVEL, __FILE__, __LINE__, __VA_ARGS__)

	#define _TFUN() base::TraceFunction tracefun(__FUNCTION__);

#else
    #define BASE_LOG_OUT(X) \
	     do{\
	     }while(0);
#endif

namespace base {

class PathHelp {
public:
	static std::string os_get_application_path();
	static std::string _base_get_name(const char* fullName);
	static std::string _base_get_file_dir(const char* fullName);
	static std::string _base_get_noext_name(const char* fullName);
private:
	PathHelp();
};

class LogFilter
{
public:
	LogFilter();
	void Init(const char* logfile);
	__inline int GetLevel() {return _level;}
protected:
	int GetLogLevel();
private:
	int _level;
	std::string _inifile;
	std::string _logname;
};

class BaseLog
{
public:
	virtual ~BaseLog(void);
	
public:
	static BaseLog* Log();
	static void Unlog();

	bool  OpenLog(const char* lpszFileName = NULL);
	__inline bool IsOpen() {return m_isOpen;}
	bool  CloseLog();
	int   WriteLog(const char* pBuf, size_t BufLen);
	void  OutFile(int loglevel, const char* file, int line, const char* lpszFormat, ...);
private:
	BaseLog(void);
	int BaseLog::GetFileLen(const char* fullName);
	bool IsEnableLog(int loglevel);
private:
	bool               m_isOpen;
	std::string        m_strFileName;
	FILE *             m_f;
	CRITICAL_SECTION   m_cs;
	static BaseLog *   m_log;
	LogFilter		   m_filter;
};

class TraceFunction {
public:
	TraceFunction(const char* procname) {
		_name = procname;
		QueryPerformanceFrequency(&_freq);
		QueryPerformanceCounter(&_begintime);
		_TRACE("Function %s Entry", _name.c_str());
	}
	~TraceFunction() {
		QueryPerformanceCounter(&_endtime);
		_TRACE("Function %s Leave, used tick:%d ms\n", _name.c_str(), 
			(_endtime.QuadPart-_begintime.QuadPart)*1000/_freq.QuadPart);
	}
private:
	std::string _name;
	LARGE_INTEGER _freq;
	LARGE_INTEGER _begintime;
	LARGE_INTEGER _endtime;
};

}