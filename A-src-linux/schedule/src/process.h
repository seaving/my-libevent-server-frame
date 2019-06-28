#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "schedule_worker.h"

/*
* 函数: process_create
* 功能: 创建进程
* 参数:	proc		子进程回调函数
*		worker		worker参数
* 返回: pid_t
*		< 0			失败
* 说明: 
*/
pid_t process_create(bool (*proc)(sch_worker_t*), sch_worker_t *worker);


#endif


