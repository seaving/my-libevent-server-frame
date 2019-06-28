#include "includes.h"

/*
* 函数: _schedule_worker_display_status
* 功能: 打印状态
* 参数: 无
* 返回: 无
* 说明: 
*/
static inline void _schedule_worker_display_status()
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
		LOG_TRACE_NORMAL("-         server port: %d\n", sch_master.conf.ser_conf.server_port);
		LOG_TRACE_NORMAL("-    listen max count: %d\n", sch_master.server.listen_count);
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

/*
* 函数: _schedule_worker_init
* 功能: 初始化
* 参数: worker		worker指针
* 返回: bool
*		- false		失败
* 说明: 
*/
static inline bool _schedule_worker_init(sch_worker_t *worker)
{
	if (worker == NULL)
	{
		return false;
	}

	worker->pid = getpid();
	worker->conn_num = 0;

	return true;
}

/*
* 函数: schedule_worker_start
* 功能: worker启动
* 参数: worker		worker指针
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_worker_start(sch_worker_t *worker)
{
	if (_schedule_worker_init(worker) == false)
	{
		return false;
	}

	if (rlimit_set_file_size() == false)
	{
		return false;
	}

	process_setaffinity(0, worker->cpu);
	LOG_TRACE_NORMAL("affinity cpu: %d\n", process_getaffinity(0));

	//log_trace_stdout_init(LOG_OUT_DIR, LOG_STDOUT_FILE);
	//log_trace_stderr_init(LOG_OUT_DIR, LOG_STDERR_FILE);

	if (tpool_create(1) < 0)
	{
		LOG_TRACE_NORMAL("create thread pool failt!\n");
		return false;
	}

	for ( ; tpool_get_worker_count() != 1; )
	{
		LOG_TRACE_NORMAL("please wait thread pool init finish (%d created).\n", 
				tpool_get_worker_count());
		sleep(1);
	}
	LOG_TRACE_NORMAL("thread pool init (%d) success.\n", 
					tpool_get_worker_count());

	if (event_service_init(1, sch_master.server.queue_size) == false)
	{
		LOG_TRACE_NORMAL("event_service_init error! exit!!!\n");
		return false;
	}

	if (schedule_server_start() == false)
	{
		LOG_TRACE_NORMAL("schedule_server_start error !\n");
		return false;
	}

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("------ child loop start ------\n");

	for ( ; ; )
	{
		_schedule_worker_display_status();
		sleep(1);
	}

	tpool_destroy();
	log_trace_stdout_exit();
	log_trace_stderr_exit();

	return true;
}

