#ifndef __LOG_TRACE_H__
#define __LOG_TRACE_H__

#include <stdio.h>
#include <string.h>
#include "version.h"
#include "user_time.h"

#define LOG_DEBUG_TRACE_ENABLE

#define DBG_PRINT	printf
#define DBG_LEVEL	""

#if defined(LOG_DEBUG_TRACE_ENABLE)
#define LOG_DEBUG_TRACE_DAEMON(format, args...) \
	do { \
		DBG_PRINT(DBG_LEVEL "%s %u [%s]:%d " format, \
			MODULE_NAME, SYSTEM_SEC, __FUNCTION__, __LINE__, ## args); \
	} while (0)
#else
#define LOG_DEBUG_TRACE_DAEMON(format, args...)
#endif

#if defined(LOG_DEBUG_TRACE_ENABLE)
#define LOG_DEBUG_TRACE_PERROR(format, args...) \
	do { \
		DBG_PRINT(DBG_LEVEL "%s %u [%s]:%d [error info(errno=%d): %s] -> " format, \
			MODULE_NAME, SYSTEM_SEC, __FUNCTION__, __LINE__, errno, strerror(errno), ## args); \
	} while (0)
#else
#define LOG_DEBUG_TRACE_PERROR(format, args...)
#endif

#define LOG_TRACE_NORMAL	LOG_DEBUG_TRACE_DAEMON
#define LOG_TRACE_PERROR	LOG_DEBUG_TRACE_PERROR
#define LOG					LOG_TRACE_NORMAL
#define PERROR				LOG_TRACE_PERROR

#define DEBUG_POINT			LOG_TRACE_NORMAL("\n");

void log_trace_stdout_init(
		char *log_dir_path, 
		char *log_file_name);

void log_trace_stderr_init(
		char *log_dir_path, 
		char *log_file_name);

void log_trace_stdout_exit();
void log_trace_stderr_exit();

#endif

