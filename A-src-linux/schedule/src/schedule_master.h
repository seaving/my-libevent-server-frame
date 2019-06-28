#ifndef __SCHEDULE_MASTER_H__
#define __SCHEDULE_MASTER_H__

#include "event_server.h"
#include "schedule_conf.h"
#include "schedule_worker.h"

#define WORKER_MAX_NUM		32

typedef struct __schedule_master__
{
	sch_conf_t conf;					//配置信息

	int worker_num;						//已经创建的工作进程数量
	sch_worker_t *workers;				//工作进程

	server_t server;
	SSL_CTX *ctx;
} sch_master_t;

/*
* 函数: schedule_master_init
* 功能: master初始化
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_master_init();

/*
* 函数: schedule_master_create_worker
* 功能: master创建子进程
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_master_create_worker();

/*
* 函数: schedule_master_create_workers
* 功能: master创建子进程
* 参数: 无
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_master_create_workers();

/********************** extern *******************/
extern sch_master_t sch_master;

#endif

