#ifndef __LOG_TRACE_H__
#define __LOG_TRACE_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "version.h"
#include "user_time.h"

#define LOG_DEBUG_TRACE_ENABLE

#define DBG_PRINT	printf

#if defined(LOG_DEBUG_TRACE_ENABLE)
#define LOG_DEBUG_TRACE(level, format, args...) \
	do { \
		DBG_PRINT("%s %u %u %s [%s]:%d " format, \
			MODULE_NAME, SYSTEM_SEC, getpid(), level, __FUNCTION__, __LINE__, ## args); \
		fflush(stdout); \
	} while (0)
#else
#define LOG_DEBUG_TRACE(format, args...)
#endif

#if defined(LOG_DEBUG_TRACE_ENABLE)
#define LOG_DEBUG_TRACE_PERROR(format, args...) \
	do { \
		DBG_PRINT("%s %u %u ERROR [%s]:%d [error info(errno=%d): %s] -> " format, \
			MODULE_NAME, SYSTEM_SEC, getpid(), __FUNCTION__, __LINE__, errno, strerror(errno), ## args); \
		fflush(stdout); \
	} while (0)
#else
#define LOG_DEBUG_TRACE_PERROR(format, args...)
#endif

#define LOG_DEBUG_TRACE_NORMAL(format, args...) LOG_DEBUG_TRACE("NORMAL", format, ## args)
#define LOG_DEBUG_TRACE_ERROR(format, args...) LOG_DEBUG_TRACE("ERROR", format, ## args)
#define LOG_DEBUG_TRACE_WARN(format, args...) LOG_DEBUG_TRACE("WARN", format, ## args)

#define LOG_TRACE_NORMAL	LOG_DEBUG_TRACE_NORMAL
#define LOG_TRACE_ERROR		LOG_DEBUG_TRACE_ERROR
#define LOG_TRACE_WARN		LOG_DEBUG_TRACE_WARN
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

