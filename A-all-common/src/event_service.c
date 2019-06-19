#include "event_service.h"
#include "sys_inc.h"
#include "log_trace.h"
#include "threadpool.h"
#include "event_worker.h"

static pthread_spinlock_t _working_count_lock;
static pthread_spinlock_t _job_wait_count_lock;
static pthread_spinlock_t _job_handling_count_lock;

static event_tpool_t *_event_service_tpool = NULL;

static void _event_service_tpool_free(event_tpool_t *tpool);

static event_tpool_t *_event_service_tpool_new(
			int worker_size, unsigned int worker_queue_size)
{
	int i = 0;
	event_tpool_t *tpool = NULL;
	
	if (worker_size > 0 && worker_queue_size > 0)
	{
		tpool = calloc(sizeof(event_tpool_t), 1);
		if (tpool == NULL)
		{
			LOG_TRACE_PERROR("calloc error!\n");
			return NULL;
		}

		tpool->workers = calloc(sizeof(event_worker_t), worker_size);
		if (tpool->workers == NULL)
		{
			LOG_TRACE_PERROR("calloc error!\n");
			_event_service_tpool_free(tpool);
			return NULL;
		}

		tpool->worker_size = worker_size;
		for (i = 0; i < worker_size; i ++)
		{
			if (event_worker_init(
					&tpool->workers[i], 
					worker_queue_size) == false)
			{
				LOG_TRACE_NORMAL("evserver_worker_init error!\n");
				_event_service_tpool_free(tpool);
				return NULL;
			}
		}

		pthread_mutex_init(&tpool->lock, NULL);
	}

	return tpool;
}

static void _event_service_tpool_free(event_tpool_t *tpool)
{
	int i = 0;
	event_worker_t *worker = NULL;
	
	if (tpool)
	{
		if (tpool->workers)
		{
			worker = tpool->workers;
			for (i = 0; i < tpool->worker_size; i ++)
			{
				event_worker_free(worker + i);
			}

			free(tpool->workers);
		}
		
		pthread_mutex_destroy(&tpool->lock);
		memset(tpool, 0, sizeof(event_tpool_t));
		free(tpool);
	}
}

bool event_service_init(int worker_size, unsigned int worker_queue_size)
{
	int i = 0;

	pthread_spin_init(&_working_count_lock, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_init(&_job_wait_count_lock, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_init(&_job_handling_count_lock, PTHREAD_PROCESS_PRIVATE);

	_event_service_tpool = _event_service_tpool_new(worker_size, worker_queue_size);
	if (_event_service_tpool == NULL)
	{
		return false;
	}

	for (i = 0; i < worker_size; i ++)
	{
		_event_service_tpool->workers[i].tpool = _event_service_tpool;
		_event_service_tpool->workers[i].tpool_index = i;
		tpool_add_work(event_worker_factory, 
					&_event_service_tpool->workers[i]);
	}

	return true;
}

void event_service_destroy()
{
	pthread_spin_destroy(&_working_count_lock);
	pthread_spin_destroy(&_job_wait_count_lock);
	pthread_spin_destroy(&_job_handling_count_lock);

	_event_service_tpool_free(_event_service_tpool);
}

int event_service_get_working_count()
{
	return _event_service_tpool ? _event_service_tpool->working_count : 0;
}

int event_service_get_job_wait_count()
{
	return _event_service_tpool ? _event_service_tpool->job_wait_count : 0;
}

int event_service_get_job_handling_count()
{
	return _event_service_tpool ? _event_service_tpool->job_handling_count : 0;
}

void event_service_working_count(int c)
{
	if (_event_service_tpool)
	{
		pthread_spin_lock(&_working_count_lock);
		_event_service_tpool->working_count += c;
		pthread_spin_unlock(&_working_count_lock);
	}
}

void event_service_job_wait_count(int c)
{
	if (_event_service_tpool)
	{
		pthread_spin_lock(&_job_wait_count_lock);
		_event_service_tpool->job_wait_count += c;
		pthread_spin_unlock(&_job_wait_count_lock);
	}
}

void event_service_job_handling_count(int c)
{
	if (_event_service_tpool)
	{
		pthread_spin_lock(&_job_handling_count_lock);
		_event_service_tpool->job_handling_count += c;
		pthread_spin_unlock(&_job_handling_count_lock);
	}
}

bool event_service_distribute_job(
		void *arg, 
		void (*free_arg_cb)(void *), 
		unsigned int timer_out, 
		executor_cb executor_cb, 
		event_cb_fn timer_cb, 
		bufferevent_data_cb read_cb, 
		bufferevent_data_cb write_cb, 
	    bufferevent_event_cb error_cb)
{
	int worker_index = 0;
	if (_event_service_tpool 
		&& _event_service_tpool->workers 
		&& arg != NULL)
	{
		pthread_mutex_lock(&_event_service_tpool->lock);

		worker_index = _event_service_tpool->current_worker 
				% _event_service_tpool->worker_size;
		_event_service_tpool->current_worker ++;
		//LOG_TRACE_NORMAL(">>>> worker_index: %d\n", worker_index);
		if (event_worker_add_job(
				&_event_service_tpool->workers[worker_index], 
				arg, free_arg_cb, timer_out, executor_cb, 
				timer_cb, read_cb, write_cb, 
				error_cb) == false)
		{
			pthread_mutex_unlock(&_event_service_tpool->lock);
			return false;
		}
		pthread_mutex_unlock(&_event_service_tpool->lock);
		return true;
	}

	return false;
}

