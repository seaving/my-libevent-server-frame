#include "includes.h"

static inline void _display_status()
{
	static int tpool_working_count = 0;
	static int tpool_worker_count = 0;

	static int evserver_working_count = 0;
	static int job_wait_count = 0;
	static int job_handling_count = 0;
    static int schedule_client_count = 0;
	
	if (tpool_working_count != tpool_get_working_count()
		|| evserver_working_count != event_service_get_working_count()
		|| job_wait_count != event_service_get_job_wait_count()
		|| job_handling_count != event_service_get_job_handling_count() 
		|| schedule_client_count != schedule_server_get_accept_client_cnt()
		)
	{
		tpool_worker_count = tpool_get_worker_count();
		tpool_working_count = tpool_get_working_count();

		evserver_working_count = event_service_get_working_count();
		job_wait_count = event_service_get_job_wait_count();
		job_handling_count = event_service_get_job_handling_count();
        schedule_client_count = schedule_server_get_accept_client_cnt();

		LOG_TRACE_NORMAL("\n");
		LOG_TRACE_NORMAL("-----------------------------------------------\n");
		LOG_TRACE_NORMAL("-             version: %s %s\n", MODULE_NAME, VERSION);
		LOG_TRACE_NORMAL("-    thread pool size: %d\n", THREAD_POOL_MAX_SIZE);
		LOG_TRACE_NORMAL("-         server port: %d\n", SCHEDULE_SERVER_BIND_PORT);
		LOG_TRACE_NORMAL("-    listen max count: %d\n", SCHEDULE_CLIENT_ACCEPT_MAX_COUNT);
		LOG_TRACE_NORMAL("-  tpool worker count: %d\n", tpool_worker_count);
		LOG_TRACE_NORMAL("- tpool working count: %d\n", tpool_working_count);
		LOG_TRACE_NORMAL("-      server workers: %d\n", evserver_working_count);
		LOG_TRACE_NORMAL("- accept client count: %d\n", schedule_client_count);
		LOG_TRACE_NORMAL("-    all jobs waiting: %d\n", job_wait_count);
		LOG_TRACE_NORMAL("-   all jobs handling: %d\n", job_handling_count);
		LOG_TRACE_NORMAL("-----------------------------------------------\n");
		LOG_TRACE_NORMAL("\n");
	}
}

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);

	time_init();

	//log_trace_stdout_init(LOG_OUT_DIR, LOG_STDOUT_FILE);
	//log_trace_stderr_init(LOG_OUT_DIR, LOG_STDERR_FILE);

	if (tpool_create(THREAD_POOL_MAX_SIZE) < 0)
	{
		LOG_TRACE_NORMAL("create thread pool failt!\n");
		return -1;
	}

	for ( ; tpool_get_worker_count() != THREAD_POOL_MAX_SIZE; )
	{
		LOG_TRACE_NORMAL("please wait thread pool init finish (%d created).\n", 
				tpool_get_worker_count());
		sleep(1);
	}
	LOG_TRACE_NORMAL("thread pool init (%d) success.\n", 
					tpool_get_worker_count());

	if (event_service_init(EVENT_WORKER_MAX_SIZE, 
			EVENT_WORKER_QUEUE_SIZE) == false)
	{
		LOG_TRACE_NORMAL("event_service_init error! exit!!!\n");
		return -1;
	}

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("------ loop start ------\n");

	event_ssl_lib_init();

	if (schedule_server_start() == false)
	{
		LOG_TRACE_NORMAL("adb_server_start error!\n");
		return -1;
	}

	for ( ; ; )
	{
		_display_status();
		sleep(1);
	}

	tpool_destroy();
	log_trace_stdout_exit();
	log_trace_stderr_exit();

	return 0;
}
