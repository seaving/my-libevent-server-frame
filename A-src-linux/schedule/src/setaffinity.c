#include "includes.h"

/*
* 函数: sys_get_cpu_num
* 功能: 获取cpu个数
* 参数: 无
* 返回: int
*		- <= 0		失败
* 说明: 
*/
int sys_get_cpu_num()
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

/*
* 函数: process_setaffinity
* 功能: 绑定进程到指定cpu
* 参数: pid			进程PID(0表示本进程)
*		cpu			cpu核序号
* 返回: bool
*		- false		失败
* 说明: 
*/
bool process_setaffinity(pid_t pid, unsigned int cpu)
{
	int cpus = 0;
	cpu_set_t mask;

	cpus = sys_get_cpu_num();
	//LOG_TRACE_NORMAL("cpu count: %d\n", cpus);

	if (cpu >= cpus)
	{
		LOG_TRACE_NORMAL("more than cpu count !\n");
		return false;
	}

	CPU_ZERO(&mask);
	CPU_SET(cpu, &mask);

	//设置cpu 亲和性(affinity)
	if (sched_setaffinity(pid, sizeof(mask), &mask) == -1)
	{
		LOG_TRACE_PERROR("sched_setaffinity error !\n");
		return false;
	}

	//让当前的设置有足够时间生效
	usleep(1000);

	return true;
}

/*
* 函数: process_getaffinity
* 功能: 获取指定进程亲和的cpu
* 参数: pid			进程ID(0表示本进程)
* 返回: int
*		- < 0		失败
* 说明: 
*/
int process_getaffinity(pid_t pid)
{
	int i = 0;
	int cpus = 0;
	cpu_set_t mask;

	cpus = sys_get_cpu_num();
	//LOG_TRACE_NORMAL("cpu count: %d\n", cpus);

	//查看当前进程的cpu亲和性
	CPU_ZERO(&mask);
	if (sched_getaffinity(pid, sizeof(mask), &mask) == -1)
	{
		LOG_TRACE_PERROR("sched_getaffinity error !\n");
		return -1;
	}

	//查看运行在当前进程的cpu
	for(i = 0; i < cpus; i ++)
	{
		//查看cpu i 是否在get集合当中
		if (CPU_ISSET(i, &mask))
		{
			LOG_TRACE_NORMAL("this process %d of running processor: %d\n", pid, i);
			return i;
		}
	}

	return -1;
}

