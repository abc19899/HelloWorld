#include "ZLogger.h"
/**
 * \brief Zebra��Ŀ��־ϵͳ�����ļ�
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
 * \brief ����һ��zLogger 
 * \param  name zLogger�����֣�����������������־�е�ÿһ��
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
 * \brief ��������
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
 * \brief �Ƴ�����̨Log���
 **/
void zLogger::removeConsoleLog()
{
	msgMut.lock();
	fp_console = NULL;
	msgMut.unlock();
}

/**
 * \brief ��һ�������ļ�Log���
 * \param file Ҫ������ļ�����Logger���Զ������ʱ���׺ 
 * \return ��
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
 * \brief ����д��־�ȼ�
 * \param  zLevelPtr ��־�ȼ�.�μ� #zLogger::zLevel
 **/
void zLogger::setLevel(const zLevel level)
{
	msgMut.lock();
	m_level = level;
	msgMut.unlock();
}

/**
 * \brief ����д��־�ȼ�
 * \param  level ��־�ȼ�
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
 * \brief д��־
 * \param  zLevelPtr ��־�ȼ��μ� #zLogger::zLevel
 * \param  pattern �����ʽ��������printfһ��
 * \return ��
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
			fprintf(*beg, "%s ���� = %u:\n",info, Datasize );
			for(int i = 0; i < Datasize; i ++)
				fprintf(*beg, "%2.2X ", pData[i]);
			fprintf(*beg, "\n\n");
			fflush(*beg);
		}
	}

	msgMut.unlock();

}
