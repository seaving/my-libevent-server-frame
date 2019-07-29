#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
	超时时间(秒):
	在此时间内IO没有任何操作的空闲时间
*/
#define CLIENT_TIMEOUT_SEC			25

/*
	线程池中线程数量
*/
#define THREAD_POOL_MAX_SIZE		1

/*
	EVENT 工作线程队列长度
*/
#define EVENT_WORKER_QUEUE_SIZE		255

/*
	schedule服务器地址
*/
#define SCHEDULE_SERVER_HOST	    "47.107.134.235"

/*
	schedule服务器端口号
*/
#define SCHEDULE_SERVER_PORT	    9111

/*
	gateway SSL证书
*/
#define SSL_CA_FILE_DIR         "/data/local/tmp"
#define	DEFAULT_CA_FILE			SSL_CA_FILE_DIR"/ssl-ca/ca.cert"
#define	DEFAULT_CRT_FILE		SSL_CA_FILE_DIR"/ssl-ca/client.crt"
#define	DEFAULT_KEY_FILE		SSL_CA_FILE_DIR"/ssl-ca/client.key"

/*
	本地服务配置
*/
#define SOCKET_LOCAL_NAME       "dzlua"

/*
	程序运行日志保存目录
*/
#define LOG_OUT_DIR					"./log"

/*
	程序运行日志文件名称
	stdout 标准输出
*/
#define LOG_STDOUT_FILE				"schedule_server_stdout.log"

/*
	程序运行日志文件名称
	stderr 标准错误输出
*/
#define LOG_STDERR_FILE				"schedule_server_stderr.log"

#endif


