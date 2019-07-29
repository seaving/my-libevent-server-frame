#include "includes.h"

sch_master_t sch_master;

/*
* 函数: _schedule_master_free
* 功能: master释放
* 参数: 无
* 返回: 无
* 说明: 
*/
static inline void _schedule_master_free()
{
	int i = 0;

	for (i = 0; i < WORKER_MAX_NUM; i ++)
	{
		if (sch_master.workers)
		{
			free(sch_master.workers);
			sch_master.workers = NULL;
		}
	}

	if (sch_master.ctx)
	{
		SSL_CTX_free(sch_master.ctx);
	}

	if (sch_master.mempool)
	{
		mem_destroy_pool(sch_master.mempool);
	}

	schedule_conf_free(&sch_master.conf);

	memset(&sch_master, 0, sizeof(sch_master_t));
}

/*
* 函数: schedule_master_init
* 功能: master初始化
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_master_init()
{
	memset(&sch_master, 0, sizeof(sch_master_t));

	event_ssl_lib_init();

	//读取配置
	schedule_conf_init(&sch_master.conf);

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("-------------------------\n");
	LOG_TRACE_NORMAL("ssl_conf->ca_file:       %s\n", sch_master.conf.ssl_conf.ca_file);
	LOG_TRACE_NORMAL("ssl_conf->crt_file:      %s\n", sch_master.conf.ssl_conf.crt_file);
	LOG_TRACE_NORMAL("ssl_conf->key_file:      %s\n", sch_master.conf.ssl_conf.key_file);
    LOG_TRACE_NORMAL("ssl_conf->ssl_enable:    %s\n", sch_master.conf.ssl_conf.ssl_enable ? "true" : "false");
	LOG_TRACE_NORMAL("ser_conf->client_max:    %d\n", sch_master.conf.ser_conf.client_max);
	LOG_TRACE_NORMAL("ser_conf->listen_count:  %d\n", sch_master.conf.ser_conf.listen_count);
	LOG_TRACE_NORMAL("ser_conf->cpu_num:       %d\n", sch_master.conf.ser_conf.cpu_num);
	LOG_TRACE_NORMAL("ser_conf->io_timeout:    %d\n", sch_master.conf.ser_conf.io_timeout);
	LOG_TRACE_NORMAL("ser_conf->server_port:   %d\n", sch_master.conf.ser_conf.server_port);
	LOG_TRACE_NORMAL("ser_conf->queue_size:    %d\n", sch_master.conf.ser_conf.queue_size);	
	LOG_TRACE_NORMAL("ser_conf->mempool_size:  %d\n", sch_master.conf.ser_conf.mempool_size);
	LOG_TRACE_NORMAL("log_conf->dir:           %s\n", sch_master.conf.log_conf.dir);
	LOG_TRACE_NORMAL("log_conf->stdout_file:   %s\n", sch_master.conf.log_conf.stdout_file);
	LOG_TRACE_NORMAL("log_conf->stderr_file:   %s\n", sch_master.conf.log_conf.stderr_file);
	LOG_TRACE_NORMAL("log_conf->record:        %s\n", sch_master.conf.log_conf.record ? "true" : "false");
	LOG_TRACE_NORMAL("-------------------------\n");
	LOG_TRACE_NORMAL("\n");

	sch_master.server.listen_fd = -1;
	sch_master.server.evbase_server = NULL;
	sch_master.server.server_ok = false;
	sch_master.server.listen_count = sch_master.conf.ser_conf.listen_count;
	sch_master.server.queue_size = sch_master.conf.ser_conf.queue_size;
	sch_master.server.listen_port = sch_master.conf.ser_conf.server_port;
	sch_master.server.accept_max = sch_master.conf.ser_conf.client_max;
	sch_master.server.io_timeout = sch_master.conf.ser_conf.io_timeout;

	if (sch_master.conf.log_conf.record)
	{
		log_trace_stdout_init(sch_master.conf.log_conf.dir, sch_master.conf.log_conf.stdout_file);
		log_trace_stderr_init(sch_master.conf.log_conf.dir, sch_master.conf.log_conf.stderr_file);
	}
#if 0
	mem_pool_init();
	sch_master.mempool = mem_create_pool(sch_master.conf.ser_conf.mempool_size);
	if (sch_master.mempool == NULL)
	{
		LOG_TRACE_ERROR("mem_create_pool error !\n");
		_schedule_master_free();
		return false;
	}
#endif
	if (sch_master.conf.ser_conf.worker_num <= 0 
		|| sch_master.conf.ser_conf.worker_num > WORKER_MAX_NUM)
	{
		LOG_TRACE_ERROR("config worker_num is null or more than %d !\n", 
			WORKER_MAX_NUM);
		_schedule_master_free();
		return false;
	}

	sch_master.workers = calloc(sizeof(sch_worker_t), WORKER_MAX_NUM);
	if (sch_master.workers == NULL)
	{
		LOG_TRACE_PERROR("calloc error !\n");
		return false;
	}

	if (sch_master.conf.ssl_conf.ssl_enable)
	{
		sch_master.ctx = event_ssl_server_init(
				sch_master.conf.ssl_conf.ca_file, 
				sch_master.conf.ssl_conf.crt_file, 
				sch_master.conf.ssl_conf.key_file);
		if (sch_master.ctx == NULL)
		{
			return false;
		}
	}

	return true;
}

/*
* 函数: schedule_master_create_worker
* 功能: master创建子进程
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_master_create_worker()
{
	pid_t pid = -1;

	if (sch_master.worker_num >= WORKER_MAX_NUM)
	{
		LOG_TRACE_ERROR("worker num is more than %d !\n", WORKER_MAX_NUM);
		return false;
	}

	sch_master.workers[sch_master.worker_num].cpu = 
		sch_master.worker_num % sch_master.conf.ser_conf.cpu_num;
	pid = process_create(schedule_worker_start, 
			&sch_master.workers[sch_master.worker_num]);
	if (pid > 0)
	{
		sch_master.workers[sch_master.worker_num].pid = pid;
		sch_master.worker_num ++;
		return true;
	}

	return false;
}

/*
* 函数: schedule_master_create_workers
* 功能: 批量master创建子进程
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_master_create_workers()
{
	int i = 0;

    if (sch_master.workers == NULL 
    	|| sch_master.conf.ser_conf.worker_num <= 0)
    {
        return false;
    }

	for (i = 0; i < sch_master.conf.ser_conf.worker_num; i ++)
	{
		schedule_master_create_worker();
	}

	return true;
}

