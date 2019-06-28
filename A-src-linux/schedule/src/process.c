#include "includes.h"

/*
* 函数: process_create
* 功能: 创建进程
* 参数:	proc		子进程回调函数
*		worker		worker参数
* 返回: pid_t
*		< 0			失败
* 说明: 
*/
pid_t process_create(bool (*proc)(sch_worker_t*), sch_worker_t *worker)
{
	pid_t pid = -1;

	if (worker == NULL)
	{
		return -1;
	}

	pid = fork();

	switch (pid)
	{
		case -1:
			LOG_TRACE_PERROR("fork error !\n");
			return -1;
		case 0:
			if (proc)
			{
				proc(worker);
			}
			break;
		default:
			break;
	}

	return pid;
}

