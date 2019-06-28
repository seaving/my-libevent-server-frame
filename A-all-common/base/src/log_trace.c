#include "sys_inc.h"
#include "log_trace.h"
#include "file.h"

typedef enum __terminal_no__
{
	TERMINAL_STDOUT = STDOUT_FILENO,
	TERMINAL_STDERR = STDERR_FILENO
} terminal_no_t;

static file_t *_log_trace_f = NULL;
static file_t *_log_trace_stderr_f = NULL;

static pthread_mutex_t _log_trace_lock = PTHREAD_MUTEX_INITIALIZER;

static file_t *_log_trace_init(
		char *log_dir_path, 
		char *log_file_name, 
		terminal_no_t terminal)
{
	file_t *f = NULL;
	char path[1024] = {0};
	mode_t old_mask = 0;

	if (log_dir_path && log_file_name)
	{
		if (create_dir(log_dir_path) == false)
		{
			LOG_TRACE_NORMAL("create dir %s failt!\n", log_dir_path);
			return NULL;
		}
		
		old_mask = umask(0);
		snprintf(path, sizeof(path) - 1, "%s%s%s", 
					log_dir_path, 
					log_dir_path[strlen(log_dir_path) - 1] == '/' ? "" : "/", 
					log_file_name);
		create_file(path);
		f = open_file(path);
		LSEEK_TO_END(f);
		umask(old_mask);

		if (FILE_FD(f) > 0)
		{
			dup2(FILE_FD(f), terminal);
			return f;
		}
	}

	file_close(f);
	return NULL;
}

static void _log_trace_exit(file_t *f)
{
	file_close(f);
}

void log_trace_lock()
{
	pthread_mutex_lock(&_log_trace_lock);
}

void log_trace_unlock()
{
	pthread_mutex_unlock(&_log_trace_lock);
}

void log_trace_stdout_init(
		char *log_dir_path, 
		char *log_file_name)
{
	_log_trace_f = _log_trace_init(
						log_dir_path, 
						log_file_name, 
						TERMINAL_STDOUT);
}

void log_trace_stderr_init(
		char *log_dir_path, 
		char *log_file_name)
{
	_log_trace_stderr_f = _log_trace_init(
						log_dir_path, 
						log_file_name, 
						TERMINAL_STDERR);
}

void log_trace_stdout_exit()
{
	_log_trace_exit(_log_trace_f);
}

void log_trace_stderr_exit()
{
	_log_trace_exit(_log_trace_stderr_f);
}


