#include "includes.h"

#define json_get(root_obj, sub_obj, key) \
	do { \
		json_object_object_get_ex(root_obj, key, &sub_obj); \
		if (sub_obj == NULL) \
		{ \
			LOG_TRACE_NORMAL("json_object_object_get error ! key = %s\n", key); \
		} \
	} while (0)

#define set_value(root_obj, value_obj, key, f)	\
	do { \
		json_get(root_obj, value_obj, key); \
		if (value_obj) \
		{ \
			f; \
		} \
	} while (0)

/*
* 函数: _schedule_conf_ssl_parse
* 功能: SSL初始化配置
* 参数: ssl_conf			缓存
*		ssl_conf_obj	json结点对象
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _schedule_conf_ssl_parse(sch_ssl_conf_t *ssl_conf, json_object *ssl_conf_obj)
{
	struct json_object *value_obj = NULL;
	if (ssl_conf == NULL 
		|| ssl_conf_obj == NULL)
	{
		return false;
	}

	set_value(ssl_conf_obj, value_obj, "ca_file", 
		ssl_conf->ca_file = strdup(json_object_get_string(value_obj)));

	set_value(ssl_conf_obj, value_obj, "crt_file", 
		ssl_conf->crt_file = strdup(json_object_get_string(value_obj)));

	set_value(ssl_conf_obj, value_obj, "key_file", 
			ssl_conf->key_file = strdup(json_object_get_string(value_obj)));

    set_value(ssl_conf_obj, value_obj, "ssl_enable", 
			ssl_conf->ssl_enable = json_object_get_boolean(value_obj));

	return true;
}

/*
* 函数: _schedule_conf_ser_parse
* 功能: server初始化配置
* 参数: ser_conf			缓存
*		ser_conf_obj	json结点对象
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _schedule_conf_ser_parse(sch_ser_conf_t *ser_conf, json_object *ser_conf_obj)
{
	struct json_object *value_obj = NULL;
	if (ser_conf == NULL 
		|| ser_conf_obj == NULL)
	{
		return false;
	}

	set_value(ser_conf_obj, value_obj, "client_max", 
		ser_conf->client_max = json_object_get_int(value_obj));

	set_value(ser_conf_obj, value_obj, "listen_count", 
		ser_conf->listen_count = json_object_get_int(value_obj));

	set_value(ser_conf_obj, value_obj, "cpu_num", 
		ser_conf->cpu_num = json_object_get_int(value_obj));

	set_value(ser_conf_obj, value_obj, "worker_num", 
		ser_conf->worker_num = json_object_get_int(value_obj));

	set_value(ser_conf_obj, value_obj, "io_timeout", 
		ser_conf->io_timeout = json_object_get_int(value_obj));

	set_value(ser_conf_obj, value_obj, "server_port", 
		ser_conf->server_port = json_object_get_int(value_obj));

	set_value(ser_conf_obj, value_obj, "queue_size", 
		ser_conf->queue_size = json_object_get_int(value_obj));

	return true;
}

/*
* 函数: _schedule_conf_log_parse
* 功能: log初始化配置
* 参数: log_conf			缓存
*		log_conf_obj	json结点对象
* 返回: bool
*		- false 		失败
* 说明: 
*/
static inline bool _schedule_conf_log_parse(sch_log_conf_t *log_conf, json_object *log_conf_obj)
{
	struct json_object *value_obj = NULL;
	if (log_conf == NULL 
		|| log_conf_obj == NULL)
	{
		return false;
	}

	set_value(log_conf_obj, value_obj, "dir", 
		log_conf->dir = strdup(json_object_get_string(value_obj)));

	set_value(log_conf_obj, value_obj, "stdout_file", 
		log_conf->stdout_file = strdup(json_object_get_string(value_obj)));

	set_value(log_conf_obj, value_obj, "stderr_file", 
		log_conf->stderr_file = strdup(json_object_get_string(value_obj)));

	set_value(log_conf_obj, value_obj, "record_enable", 
		log_conf->record = json_object_get_boolean(value_obj));

	return true;
}

/*
* 函数: schedule_conf_init
* 功能: 初始化配置
* 参数: conf		缓存
* 返回: bool
*		- false 失败
* 说明: 
*/
bool schedule_conf_init(sch_conf_t *conf)
{
	json_object *ssl_conf_obj = NULL;
	json_object *ser_conf_obj = NULL;
	json_object *log_conf_obj = NULL;
	json_object *json_obj = NULL;

	if (conf == NULL)
	{
		return false;
	}

	json_obj = json_object_from_file(SCHEDULE_CONF_FILE);
	if (json_obj == NULL)
	{
		LOG_TRACE_NORMAL("json_object_from_file error ! file: %s\n", 
				SCHEDULE_CONF_FILE);
		return false;
	}

	json_get(json_obj, ssl_conf_obj, "ssl_conf");
	json_get(json_obj, ser_conf_obj, "ser_conf");
	json_get(json_obj, log_conf_obj, "log_conf");

	_schedule_conf_ssl_parse(&conf->ssl_conf, ssl_conf_obj);
	_schedule_conf_ser_parse(&conf->ser_conf, ser_conf_obj);
	_schedule_conf_log_parse(&conf->log_conf, log_conf_obj);

	json_object_put(json_obj);
	return true;
}

/*
* 函数: schedule_conf_free
* 功能: 配置存储缓存释放
* 参数: 无
* 返回: 无
* 说明: 
*/
void schedule_conf_free(sch_conf_t *conf)
{
	if (conf)
	{
		safe_free(conf->ssl_conf.ca_file);
		safe_free(conf->ssl_conf.crt_file);
		safe_free(conf->ssl_conf.key_file);

		safe_free(conf->log_conf.dir);
		safe_free(conf->log_conf.stderr_file);
		safe_free(conf->log_conf.stdout_file);
	}
}

#undef set_value
#undef json_get

