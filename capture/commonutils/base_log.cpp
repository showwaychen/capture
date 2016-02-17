#include "stdafx.h"
#include "base_log.h"
#include <shlobj.h>


#define LOGBUF_LEN		256*4
char g_format_buf[LOGBUF_LEN];
char g_out_buf[LOGBUF_LEN];
char g_desc[32] = {"\0"};


#define LOG_DESC_ALL		"ALL"
#define LOG_DESC_ERR		"ERROR"
#define LOG_DESC_WARNING	"WARNING"
#define LOG_DESC_TRACE		"TRACE"
#define LOG_DESC_INFO		"INFO"

namespace base {

BaseLog * BaseLog::m_log = NULL;
/*
[FILTER]
PNPLOG
*/
LogFilter::LogFilter() {
	_level = DEFAULT_LEVEL;
}

void LogFilter::Init(const char* logfile)
{
	if (logfile!= NULL)
	{
		_logname = PathHelp::_base_get_name(logfile);
		_inifile = PathHelp::_base_get_file_dir(logfile);
		_inifile += "\\log.ini";
		if ((_level = GetLogLevel())<0)
			_level = DEFAULT_LEVEL;
	}
}

int LogFilter::GetLogLevel() {
	char buffer[32] = {'\0'};
	GetPrivateProfileStringA("filter", _logname.c_str(), "-1", buffer, 32, _inifile.c_str());
	return atoi(buffer);
}

BaseLog::BaseLog(void) {
	m_isOpen      = false;
	m_f           = NULL;
	m_strFileName = "";
	::InitializeCriticalSection( &m_cs );
}

BaseLog::~BaseLog(void) {
	::DeleteCriticalSection( &m_cs );
}

BaseLog * BaseLog::Log() {
	if( m_log == NULL )
		m_log = new BaseLog;
	return m_log;
}

void BaseLog::Unlog() {
	if (m_log != NULL) {
		m_log->CloseLog();
		delete m_log;
		m_log = NULL;
	}
}

std::string GetCurrentModuleName() {
	std::string name;
	char buffer[MAX_PATH];
	HMODULE hModule = NULL;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
		(LPCSTR)&GetCurrentModuleName, //这是函数名，强转
		&hModule);
	DWORD len = GetModuleFileNameA(hModule, buffer, MAX_PATH);
	if (len) {
		name =  PathHelp::_base_get_noext_name(buffer);
	}
	return name;
}

bool BaseLog::OpenLog(const char* lpszFileName) {
	if (m_f != NULL)
		return false;
	if (lpszFileName == NULL) {
		m_strFileName  = PathHelp::os_get_application_path();
		m_strFileName += "log\\";
		m_strFileName += GetCurrentModuleName();
		m_strFileName += ".log";
	} else {
		m_strFileName  = PathHelp::os_get_application_path();
		m_strFileName += lpszFileName;
	}

	int nSize = GetFileLen( m_strFileName.c_str() );
	if (nSize > 1024*1024*5)
		remove(m_strFileName.c_str());

	m_f = fopen (m_strFileName.c_str(), "a+");
	if (!m_f) {
		m_isOpen = false;
		return false;
	}
	m_isOpen = true;
	m_filter.Init(m_strFileName.c_str());
	return true;
}

bool BaseLog::CloseLog() {
	::EnterCriticalSection( &m_cs );
	if( m_f != NULL ) {
		fclose( m_f );
		m_f = NULL;
		m_isOpen = false;
	}
	::LeaveCriticalSection( &m_cs );
	return true;
}

int BaseLog::WriteLog( const char* pBuf, size_t BufLen) {
	::EnterCriticalSection(&m_cs);
	int nSize = fwrite( pBuf, sizeof(char), BufLen, m_f);
	fflush(m_f);
	::LeaveCriticalSection(&m_cs);
	return nSize;
}

char* LogLevelString(int loglevel) {
	g_desc[0]='\0';
	if (loglevel == LOG_LEVEL_ALL)
		strcpy(g_desc, LOG_DESC_ALL);
	else if ((loglevel & LOG_LEVEL_ERROR))
		strcpy(g_desc, LOG_DESC_ERR);
	else if ((loglevel & LOG_LEVEL_WARNING))
		strcpy(g_desc, LOG_DESC_WARNING);
	else if ((loglevel & LOG_LEVEL_TRACE))
		strcpy(g_desc, LOG_DESC_TRACE);
	else if ((loglevel & LOG_LEVEL_INFO))
		strcpy(g_desc, LOG_DESC_INFO);
	return g_desc;
}

bool BaseLog::IsEnableLog(int loglevel) {
#ifndef LOG_NOFILTER
	bool enable_log = false;
	if ((loglevel & LOG_LEVEL_ERROR))
		enable_log = ((m_filter.GetLevel() & LOG_LEVEL_ERROR) >0);
	else if ((loglevel & LOG_LEVEL_WARNING))
		enable_log = ((m_filter.GetLevel() & LOG_LEVEL_WARNING) >0);
	else if ((loglevel & LOG_LEVEL_TRACE))
		enable_log = ((m_filter.GetLevel() & LOG_LEVEL_TRACE) >0);
	else if ((loglevel & LOG_LEVEL_INFO))
		enable_log = ((m_filter.GetLevel() & LOG_LEVEL_INFO) >0);
	else if (loglevel == LOG_LEVEL_ALL)
		enable_log = true;

	return enable_log;
#else
	return true;
#endif
}

void BaseLog::OutFile(int loglevel, const char* file, int line, const char* lpszFormat, ... ) {
	if (file) {
		// log only the filename
		const char* last_slash = strrchr(file, '\\');
		if (last_slash)
			file = last_slash + 1;
	}

	va_list args;
	va_start(args, lpszFormat);
	_vsnprintf(g_format_buf, LOGBUF_LEN, lpszFormat, args);
	va_end(args);
	
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	if (file) {
		sprintf_s(g_out_buf, 
			LOGBUF_LEN, 
			"[%s|%d-%d-%d %d:%d:%d(%s:%d)] %s\n", 
			LogLevelString(loglevel),
			sysTime.wYear, 
			sysTime.wMonth,
			sysTime.wDay,
			sysTime.wHour, 
			sysTime.wMinute, 
			sysTime.wSecond,
			file,
			line,
			g_format_buf);
	} else {
		sprintf_s(g_out_buf, 
			LOGBUF_LEN, 
			"[%s|%d-%d-%d %d:%d:%d] %s\n", 
			LogLevelString(loglevel),
			sysTime.wYear, 
			sysTime.wMonth,
			sysTime.wDay,
			sysTime.wHour, 
			sysTime.wMinute, 
			sysTime.wSecond,
			g_format_buf);
	}
	
	if (IsEnableLog(loglevel)) {
	#ifdef _DEBUG
		OutputDebugStringA(g_out_buf);
	#endif
		if( !IsOpen() ) 
			return;
		WriteLog( g_out_buf, strlen(g_out_buf));
	}
}

int BaseLog::GetFileLen(const char* fullName) {
	FILE * f = fopen(fullName, "r");
	if (f != NULL) {
		fseek(f, 0, SEEK_END);
		//获取文件指针的位置，从而取得文件大小
		int nSize = ftell(f); 
		fclose(f);
		return nSize;
	}
	return 0;
}

std::string PathHelp::os_get_application_path() {
	char appfname[256];
	::GetModuleFileNameA( NULL, appfname, 256 );
	char *p = ::strrchr(appfname, '\\' );
	if (!p)
		p = ::strrchr(appfname, '/' );

	if (p)
		*(p+1) = 0;

	return std::string(appfname);
}

std::string PathHelp::_base_get_name(const char* fullName) {
	int nLen = strlen(fullName);
	for( int i = nLen-1; i >= 0; i-- ) {
		if( fullName[i] == '\\' || fullName[i] == '/' )
			return std::string(&fullName[i+1]);
	}
	return "";
}

std::string PathHelp::_base_get_file_dir(const char* fullName) {
	const char* pBegin  = fullName;
	const char* pEnd    = NULL;
	int nLen = strlen( fullName );
	for (int i = nLen-1; i >= 0; i-- ) {
		if( fullName[i] == '\\' || fullName[i] == '/' ) {
			pEnd = &fullName[i];
			break;
		}
	}
	if (pEnd != NULL)
		return std::string(pBegin, nLen-strlen(pEnd));
	else
		return "";
}

std::string PathHelp::_base_get_noext_name(const char* fullName) {
	std::string strName = _base_get_name(fullName);
	size_t pos = strName.rfind(".");
	if (pos != strName.npos) {
		strName.erase(pos);
		return strName;
	}
	return strName;
}

}