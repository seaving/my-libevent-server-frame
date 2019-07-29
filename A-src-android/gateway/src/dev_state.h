#ifndef __DEV_STATE_H__
#define __DEV_STATE_H__

typedef struct __cpu_state__
{
	int cores;		//核心数
	int idle;		//空闲百分比
} cpu_state_t;

typedef struct __battery_state__
{
	int capacity;	//容量
	int residual;	//剩余百分比
} battery_state_t;

typedef struct __task_state__
{
	bool idle;		//是否空闲
} task_state_t;

typedef struct __dev_state__
{
	battery_state_t battery;
	cpu_state_t cpu;
	task_state_t task;
} dev_state_t;

extern dev_state_t dev_state;

#endif

