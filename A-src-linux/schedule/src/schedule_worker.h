#ifndef __SCHEDULE_WORKER_H__
#define __SCHEDULE_WORKER_H__

typedef struct __schedule_worker__
{
	char name[64];						//名称
	pid_t pid;							//子进程PID
	int cpu;							//亲和cpu序号
	int status;							//状态
	
	int conn_num;						//连接数

	unsigned respawn;					//重生标记
    unsigned exiting:1;					//正在退出标记
    unsigned exited:1;					//已经退出

} sch_worker_t;

/*
* 函数: schedule_worker_start
* 功能: worker启动
* 参数: worker		worker指针
* 返回: bool
*		- false		失败
* 说明: 
*/
bool schedule_worker_start(sch_worker_t *worker);

#endif

