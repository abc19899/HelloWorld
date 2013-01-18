#ifndef _Z_LOGGER_H_
#define _Z_LOGGER_H_

#include <iostream>
#include <boost/thread/mutex.hpp>

typedef unsigned char BYTE;
//	static const char *LEVEL_STRING[] = {
//		"LEVEL_OFF",
//		"LEVEL_FATAL",
//		"LEVEL_ERROR",
//		"LEVEL_WARN",
//		"LEVEL_DEBUG",
//		"LEVEL_INFO",
//		"LEVLE_ALL"
//	};
/**
 * \brief Zebra项目的日志类。
 * 目前实现了两种写日志方式，即控制台、本地文件。
 * 默认日志级别是#DEBUG
 * 此类为线程安全类。
 **/
class zLogger
{
public:
	/**
 	 * \brief zLevel声明了几个日志等级
 	 * 除了用log4cxx提供的标准日志等级作为日志等级外，还自定义了游戏日志等级.
 	 * 程序日志等级关系为 #OFF> #FATAL> #ERROR> #WARN> #DEBUG> #INFO> #ALL
 	 **/

	/**
 	 * \brief Zebra项目所支持日志等级数字定义
 	 **/
	typedef enum
	{
		/**
 		 * \brief 当zLogger等级设置为OFF，否则不会输出任何日志
 		 **/
		LEVEL_OFF   = INT_MAX,

		/**
 		 * \brief 当zLogger等级设置为FATAL，只输出FATAL等级的日志
 		 * 程序致命错误，已经无法提供正常的服务功能。
 		 **/
		LEVEL_FATAL = 50000,

		/**
 		 * \brief 当zLogger等级设置为ERROR，只输出大于等于此等级的日志
 		 * 错误，可能不能提供某种服务，但可以保证程序正确运行。
 		 **/
		LEVEL_ERROR = 40000,

		/**
 		 * \brief 当zLogger等级设置为WARN，只输出大于等于此等级的日志
 		 * 警告，某些地方需要引起注意，比如没有配置文件，但程序用默认选项可以使用。
 		 **/
		LEVEL_WARN  = 30000,

		/**
 		 * \brief 当zLogger等级设置为DEBUG，输出所有等级的日志
 		 **/
		LEVEL_DEBUG = 20000,

		/**
 		 * \brief 当zLogger等级设置为INFO，只输出大于等于此等级的日志
 		 * 信息，提供一般信息记录，多用于一些程序状态的记录。
 		 **/
		LEVEL_INFO  = 10000,

		/**
 		 * \brief 当zLogger等级设置为ALL，输出所有等级的日志
 		 **/
		LEVEL_ALL   = INT_MIN
	}zLevel;

	const char *getZLevelString(zLevel level)
	{
		switch(level)
		{
		case LEVEL_OFF:
			return "OFF";
		case LEVEL_FATAL:
			return "FATAL";
		case LEVEL_ERROR:
			return "ERROR";
		case LEVEL_WARN:
			return "WARN";
		case LEVEL_DEBUG:
			return "DEBUG";
		case LEVEL_INFO:
			return "INFO";
		case LEVEL_ALL:
			return "ALL";
		default:
			return "NO_LEVEL";
		}
	}
	zLogger(char *name = "Zebra" ,const char *file = "log");
	~zLogger();

	void removeConsoleLog();
	void addLocalFileLog(const std::string &file);

	void setLevel(const zLevel level);
	void setLevel(const std::string &level);

	void logva(const zLevel level, const char* file, int line,
		const char * pattern,va_list vp);
	void log(const zLevel level, const char* file, int line, 
		const char * pattern, ...);
	void log16(const zLevel level, const char* file, int line, 
		const char* info, const BYTE* pData, int Datasize);

private:
	boost::mutex	msgMut;

	zLevel      	m_level;
	FILE*			fp_console;
	FILE*			fp_file;
	int         	m_day;
	std::string 	m_name;
	std::string 	m_file;
};

namespace Zebra
{
	/**
 	 * \brief 日志指针
 	 **/
	extern zLogger *logger;
};

#define ZLOG_FATAL(format, ...) \
Zebra::logger->log(zLogger::LEVEL_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ZLOG_ERROR(format, ...) \
Zebra::logger->log(zLogger::LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ZLOG_WARN(format, ...) \
Zebra::logger->log(zLogger::LEVEL_WARN,  __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ZLOG_DEBUG(format, ...) \
Zebra::logger->log(zLogger::LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ZLOG_INFO(format, ...) \
Zebra::logger->log(zLogger::LEVEL_INFO,  __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ZLOG_DEBUG16(info, pData, Datasize) \
Zebra::logger->log16(zLogger::LEVEL_DEBUG, __FILE__, __LINE__, info, pData, Datasize)


#endif // _Z_LOGGER_H_
