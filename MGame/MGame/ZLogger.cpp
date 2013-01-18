#include "ZLogger.h"
/**
 * \brief Zebra项目日志系统定义文件
 **/

#include "ZLogger.h"
#include <vector>
#include <stdarg.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/syscall.h>
#define GetCurrentThreadId() syscall(SYS_gettid)
#endif
//#include "srvEngine.h"


namespace Zebra
{
	zLogger *logger = NULL;
};

/**
 * \brief 构造一个zLogger 
 * \param  name zLogger的名字，将会出现在输出的日志中的每一行
 **/
zLogger::zLogger(char *name, const char *file)
  :	m_level(LEVEL_DEBUG)
  ,	fp_console(stdout)
  ,	fp_file(NULL)
  ,	m_day(0)
  ,	m_name(name)
//  , m_file(file)
{
	m_file = std::string("log/") + file;
}

/**
 * \brief 析构函数
 **/
zLogger::~zLogger()
{
	if (NULL != fp_file)
	{
		fclose(fp_file);
		fp_file = NULL;
	}
}

/**
 * \brief 移除控制台Log输出
 **/
void zLogger::removeConsoleLog()
{
	msgMut.lock();
	fp_console = NULL;
	msgMut.unlock();
}

/**
 * \brief 加一个本地文件Log输出
 * \param file 要输出的文件名，Logger会自动地添加时间后缀 
 * \return 无
 **/
void zLogger::addLocalFileLog(const std::string &file)
{
	msgMut.lock();
	m_day  = 0;
	//m_file = file;
	m_file = std::string("log/") + file;
	msgMut.unlock();
}

/**
 * \brief 设置写日志等级
 * \param  zLevelPtr 日志等级.参见 #zLogger::zLevel
 **/
void zLogger::setLevel(const zLevel level)
{
	msgMut.lock();
	m_level = level;
	msgMut.unlock();
}

/**
 * \brief 设置写日志等级
 * \param  level 日志等级
 **/
void zLogger::setLevel(const std::string &level)
{
	if ("off" == level) 		setLevel(LEVEL_OFF);
	else if ("fatal" == level) 	setLevel(LEVEL_FATAL);
	else if ("error" == level) 	setLevel(LEVEL_ERROR);
	else if ("warn" == level) 	setLevel(LEVEL_WARN);
	else if ("debug" == level) 	setLevel(LEVEL_DEBUG);
	else if ("info" == level) 	setLevel(LEVEL_INFO);
	else if ("all" == level) 	setLevel(LEVEL_ALL);
	else
	{
		std::cerr << "zLogger::setLevel, unknown input(" << level
		<< ")" << std::endl;
		assert(0);
	}
}

void zLogger::logva(const zLevel level, const char* file, int line,
	const char * pattern,va_list vp)
{
	if (m_level > level) return;

	std::string strFile(file);
#ifdef WIN32
	std::string::size_type pos = strFile.find_last_of('\\');
#else
	std::string::size_type pos = strFile.find_last_of("/");
#endif
	if(pos == std::string::npos) 
		pos = 0;
	else
		++ pos;

#ifdef WIN32
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);		

	char szName[255];
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm* now = localtime(&tv.tv_sec);

	char   szName[sysconf(_PC_PATH_MAX)];
#endif

	msgMut.lock();

	if (!m_file.empty())
	{
#ifdef WIN32
		if(m_day != sysTime.wDay)
#else
		if (m_day != now->tm_mday)
#endif
		{
			if (NULL != fp_file)
			{
				fclose(fp_file);
			}
#ifdef WIN32
			m_day = sysTime.wDay;
			sprintf_s(szName, sizeof(szName), "%s%04d%02d%02d.log",m_file.c_str(),
				sysTime.wYear, sysTime.wMonth, sysTime.wDay);
#else
			m_day = now->tm_mday;
			snprintf(szName,sizeof(szName),"%s%04d%02d%02d.log",m_file.c_str(),
				now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
#endif
			fp_file = fopen(szName,"at");
		}
	}

	std::vector<FILE*> v_ptrFile;
	v_ptrFile.push_back(fp_console);
	v_ptrFile.push_back(fp_file);

	for(std::vector<FILE*>::iterator beg = v_ptrFile.begin();
		beg != v_ptrFile.end(); ++beg)
	{
		if (NULL != *beg)
		{
			fprintf(*beg,"[%s][%ld] ",m_name.c_str(), GetCurrentThreadId());
#ifdef WIN32
			fprintf(*beg, "%04d/%02d/%02d ", sysTime.wYear,
				sysTime.wMonth, sysTime.wDay);
			fprintf(*beg, "%02d:%02d:%02d.%03ld ", sysTime.wHour,
				sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
#else
			fprintf(*beg, "%04d/%02d/%02d ",now->tm_year + 1900,
				now->tm_mon + 1, now->tm_mday);
			fprintf(*beg, "%02d:%02d:%02d.%03ld ", now->tm_hour,
				now->tm_min, now->tm_sec, tv.tv_usec / 1000);
#endif
			fprintf(*beg,"{%s}(%d) ", strFile.substr(pos).c_str(), line);

			fprintf(*beg, "[%s] ", zLogger::getZLevelString(level));
			vfprintf(*beg, pattern, vp);
			fprintf(*beg, "\n");
			fflush(*beg);
		}
	}

	msgMut.unlock();
}

/**
 * \brief 写日志
 * \param  zLevelPtr 日志等级参见 #zLogger::zLevel
 * \param  pattern 输出格式范例，与printf一样
 * \return 无
 **/
void zLogger::log(const zLevel level, const char* file, int line,
	const char * pattern,...)
{
	va_list vp;

	if (m_level > level) return;
	va_start(vp,pattern);
	logva(level, file, line, pattern, vp);
	va_end(vp);
}

void zLogger::log16(const zLevel level, const char* file, int line,
	const char* info, const BYTE* pData, int Datasize)
{	
	if (m_level > level) return;

	std::string strFile(file);
#ifdef WIN32
	std::string::size_type pos = strFile.find_last_of('\\');
#else
	std::string::size_type pos = strFile.find_last_of('/');
#endif
	if(pos == std::string::npos)
		pos = 0;
	else
		++ pos;

#ifdef WIN32
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);		

	char szName[255];
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm* now = localtime(&tv.tv_sec);

	char   szName[sysconf(_PC_PATH_MAX)];
#endif

	msgMut.lock();

	if (!m_file.empty())
	{
#ifdef WIN32
		if(m_day != sysTime.wDay)
#else
		if (m_day != now->tm_mday)
#endif
		{
			if (NULL != fp_file)
			{
				fclose(fp_file);
			}
#ifdef WIN32
			m_day = sysTime.wDay;
			sprintf_s(szName, sizeof(szName), "%s%04d%02d%02d.log",m_file.c_str(),
				sysTime.wYear, sysTime.wMonth, sysTime.wDay);
#else
			m_day = now->tm_mday;
			snprintf(szName,sizeof(szName),"%s%04d%02d%02d.log",m_file.c_str(),
				now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
#endif
			fp_file = fopen(szName,"at");
		}
	}

	std::vector<FILE*> v_ptrFile;
	v_ptrFile.push_back(fp_console);
	v_ptrFile.push_back(fp_file);

	for(std::vector<FILE*>::iterator beg = v_ptrFile.begin();
		beg != v_ptrFile.end(); ++beg)
	{
		if (NULL != *beg)
		{
//			fprintf(*beg,"[%s][%ld] ",m_name.c_str(), GetCurrentThreadId());
//			fprintf(*beg, "%04d/%02d/%02d ",now->tm_year + 1900,
//				now->tm_mon + 1, now->tm_mday);
//			fprintf(*beg, "%02d:%02d:%02d.%03ld ", now->tm_hour,
//				now->tm_min, now->tm_sec, tv.tv_usec / 1000);
//			fprintf(*beg,"{%s}(%d) ", strFile.substr(pos).c_str(), line);
			fprintf(*beg,"[%s][%ld] ",m_name.c_str(), GetCurrentThreadId());
#ifdef WIN32
			fprintf(*beg, "%04d/%02d/%02d ", sysTime.wYear,
				sysTime.wMonth, sysTime.wDay);
			fprintf(*beg, "%02d:%02d:%02d.%03ld ", sysTime.wHour,
				sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
#else
			fprintf(*beg, "%04d/%02d/%02d ",now->tm_year + 1900,
				now->tm_mon + 1, now->tm_mday);
			fprintf(*beg, "%02d:%02d:%02d.%03ld ", now->tm_hour,
				now->tm_min, now->tm_sec, tv.tv_usec / 1000);
#endif
			fprintf(*beg,"{%s}(%d) ", strFile.substr(pos).c_str(), line);

			fprintf(*beg, "[%s] ", zLogger::getZLevelString(level));
			fprintf(*beg, "%s 长度 = %u:\n",info, Datasize );
			for(int i = 0; i < Datasize; i ++)
				fprintf(*beg, "%2.2X ", pData[i]);
			fprintf(*beg, "\n\n");
			fflush(*beg);
		}
	}

	msgMut.unlock();

}
