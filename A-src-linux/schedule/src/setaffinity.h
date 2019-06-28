#ifndef __SETAFFINITY_H__
#define __SETAFFINITY_H__

#include "sys_inc.h"

/*
* 函数: sys_get_cpu_num
* 功能: 获取cpu个数
* 参数: 无
* 返回: int
*		- <= 0		失败
* 说明: 
*/
int sys_get_cpu_num();

/*
* 函数: process_setaffinity
* 功能: 绑定进程到指定cpu
* 参数: pid			进程PID
*		cpu			cpu核序号
* 返回: bool
*		- false		失败
* 说明: 
*/
bool process_setaffinity(pid_t pid, unsigned int cpu);

/*
* 函数: process_getaffinity
* 功能: 获取指定进程亲和的cpu
* 参数: pid			进程ID
* 返回: int
*		- < 0		失败
* 说明: 
*/
int process_getaffinity(pid_t pid);

#endif

