#ifndef __SCHEDULE_CONF_H__
#define __SCHEDULE_CONF_H__

#define SCHEDULE_CONF_FILE		"/etc/cloud_ctrl/config/schedule.conf"

typedef struct __schedule_ssl_conf__
{
	char *ca_file;
	char *crt_file;
	char *key_file;
	bool ssl_enable;
} sch_ssl_conf_t;

typedef struct __schedule_server_conf__
{
	int cpu_num;						//服务器CPU核数
	int worker_num; 					//worker数量
	int server_port;					//服务器端口
	int client_max;						//最大连接数上限
	int listen_count;					//监听队列长度
	int io_timeout;						//IO超时时间(秒)
	int queue_size;						//队列长度
} sch_ser_conf_t;

typedef struct __schedule_log_conf__
{
	char *dir;							//日志保存目录
	char *stdout_file;					//标准输出文件名
	char *stderr_file;					//标准错误文件名
	bool record;						//是否输出到文件
} sch_log_conf_t;

typedef struct __schedule_conf__
{
	sch_ser_conf_t ser_conf;
	sch_ssl_conf_t ssl_conf;
	sch_log_conf_t log_conf;
} sch_conf_t;

/*
* 函数: schedule_conf_init
* 功能: 初始化配置
* 参数: conf		缓存
* 返回: bool
*		- false 失败
* 说明: 
*/
bool schedule_conf_init(sch_conf_t *conf);

/*
* 函数: schedule_conf_free
* 功能: 配置存储缓存释放
* 参数: 无
* 返回: 无
* 说明: 
*/
void schedule_conf_free(sch_conf_t *conf);

#endif

