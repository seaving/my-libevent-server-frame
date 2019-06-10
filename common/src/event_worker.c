#include "event_worker.h"
#include "sys_inc.h"
#include "log_trace.h"
#include "event_service.h"
#include "event_executor.h"

static inline bool _event_worker_job_init(
		event_worker_job_t *job, 
		void *arg, 
		void (*free_arg_cb)(void *), 
		unsigned int timer_out, 
		executor_cb executor_cb, 
		event_cb_fn timer_cb, 
		bufferevent_data_cb read_cb, 
		bufferevent_data_cb write_cb, 
		bufferevent_event_cb error_cb)
{
	if (job && arg)
	{
		memset(job, 0, sizeof(event_worker_job_t));

		job->arg = arg;
		job->free_arg_cb = free_arg_cb;
		job->timer_out = timer_out;
		job->executor_cb = executor_cb;
		job->read_cb = read_cb;
		job->write_cb = write_cb;
		job->timer_cb = timer_cb;
		job->error_cb = error_cb;

		return true;
	}

	return false;
}

static inline void _event_worker_job_free(void *data)
{
	event_worker_job_t *job = (event_worker_job_t *) data;

	if (job)
	{
		if (job->processing == false
			&& job->free_arg_cb)
		{
			job->free_arg_cb(job->arg);
		}

		free(job);
	}
}

static void _event_worker_add_event(event_worker_t *worker)
{
	int i = 0;
	item_t *item = NULL;
	event_worker_job_t *job = NULL;
	bool ret = false;

	for (i = 0; worker && i < 1; i ++)
	{
		item = NULL;
		job = NULL;

		item = queue_pop(worker->queue);
		job = (event_worker_job_t *) item_get_data(worker->queue, item);
		if (job == NULL)
		{
			//LOG_TRACE_NORMAL("queue_pop error!\n");
			item_free(worker->queue, item);
			return;
		}
		job->processing = false;

		event_service_job_wait_count(-1);

		//LOG_TRACE_NORMAL("------------------------\n");
		//LOG_TRACE_NORMAL("connect_fd: %d\n", job.connect_fd);
		//LOG_TRACE_NORMAL("timer_cb: %u\n", (unsigned int) job.timer_cb);
		//LOG_TRACE_NORMAL("read_cb: %u\n", (unsigned int) job.read_cb);
		//LOG_TRACE_NORMAL("write_cb: %u\n", (unsigned int) job.write_cb);
		//LOG_TRACE_NORMAL("error_cb: %u\n", (unsigned int) job.error_cb);
		//LOG_TRACE_NORMAL("worker_cb: %u\n", (unsigned int) job.worker_cb);
		//LOG_TRACE_NORMAL("------------------------\n");

		if (job->arg && job->executor_cb)
		{
			ret = job->executor_cb(worker->evbase, worker, 
						job->arg, job->free_arg_cb, job->timer_out, job->timer_cb, 
						job->read_cb, job->write_cb, 
						job->error_cb);
			if (ret == false)
			{
				//不需要做处理，内部已经有错误处理
			}
			else
			{
				job->processing = true;
				event_service_job_handling_count(1);
			}
		}
		else
		{
			if (job->free_arg_cb)
			{
				job->free_arg_cb(job->arg);
			}
		}

		job->free_arg_cb = NULL;
		item_free(worker->queue, item);
	}
}

static void _event_worker_read_cb(
				evutil_socket_t fd, short ev, void *arg)
{
	char buf[2] = {0};
	event_worker_t *worker = (event_worker_t *) arg;
	if (worker == NULL)
	{
		return;
	}

	if (fd != worker->read_fd)
	{
		LOG_TRACE_NORMAL("OH MY GOD! The fd(%d) not my read fd(%d)! exit!!!!\n", 
					fd, worker->read_fd);
		exit(-1);
	}

	/*LOG_TRACE_NORMAL("[worker_index=%d] new notification processing ...\n", 
			worker->tpool_index);*/
	
	recv(worker->read_fd, buf, 1, 0);
	_event_worker_add_event(worker);
}

bool event_worker_init(event_worker_t *worker, unsigned int queue_size)
{
	int fd[2] = {-1, -1};

	if (worker == NULL)
	{
		return false;
	}

	memset(worker, 0, sizeof(event_worker_t));
	
	worker->queue = queue_create();
	if (worker->queue == NULL)
	{
		LOG_TRACE_NORMAL("queue_create error!\n");
		return false;
	}

	queue_set_depth(worker->queue, queue_size);
	queue_set_hook(worker->queue, NULL, _event_worker_job_free);

	pthread_mutex_init(&worker->lock, NULL);
	
	if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
	{
		LOG_TRACE_PERROR("evutil_socketpair error!\n");
		return false;
	}

	evutil_make_socket_closeonexec(fd[0]);
	evutil_make_socket_closeonexec(fd[1]);

	evutil_make_socket_nonblocking(fd[0]);
	evutil_make_socket_nonblocking(fd[1]);
	
	worker->write_fd = fd[0];
	worker->read_fd = fd[1];
	worker->evbase = event_base_new();
	if (worker->evbase == NULL)
	{
		LOG_TRACE_NORMAL("event_base_new error!\n");
		return false;
	}

	worker->evdnsbase = evdns_base_new(worker->evbase, 1);
	if (worker->evdnsbase == NULL)
	{
		LOG_TRACE_NORMAL("evdns_base_new error!\n");
		return false;
	}

	if (event_assign(&worker->event, 
			worker->evbase, worker->read_fd,
			EV_READ | EV_PERSIST, 
			_event_worker_read_cb, worker) < 0)
	{
		LOG_TRACE_NORMAL("event_assign error!\n");
		return false;
	}

	if (event_add(&worker->event, NULL) < 0)
	{
		LOG_TRACE_NORMAL("event_add error!\n");
		return false;
	}
	
	return true;
}

event_worker_t *event_worker_new(unsigned int fifo_size)
{
	event_worker_t *worker = calloc(sizeof(event_worker_t), 1);
	if (worker == NULL)
	{
		LOG_TRACE_PERROR("calloc error!\n");
		return NULL;
	}

	if (event_worker_init(worker, fifo_size) == false)
	{
		event_worker_free(worker);
		return NULL;
	}

	worker->create_by_malloc = true;
	
	return worker;
}

void event_worker_free(event_worker_t *worker)
{
	if (worker)
	{
		if (worker->evdnsbase)
		{
			evdns_base_free(worker->evdnsbase, 0);
			worker->evdnsbase = NULL;
		}

		if (worker->evbase)
		{
			event_del(&worker->event);
			event_base_loopexit(worker->evbase, NULL);
			event_base_free(worker->evbase);
			worker->evbase = NULL;
		}

		if (worker->read_fd > 0)
		{
			close(worker->read_fd);
		}
		if (worker->write_fd > 0)
		{
			close(worker->write_fd);
		}

		if (worker->queue)
			queue_destroy(worker->queue);
		
		pthread_mutex_destroy(&worker->lock);

		memset(worker, 0, sizeof(event_worker_t));

		if (worker->create_by_malloc)
		{
			free(worker);
		}
	}
}

void *event_worker_factory(void *arg)
{
	int tpool_index = 0;
	event_tpool_t *tpool = NULL;
	event_worker_t *worker = (event_worker_t *) arg;
	if (worker == NULL
		|| worker->evbase == NULL)
	{
		return NULL;
	}

	tpool = (event_tpool_t *) worker->tpool;
	if (tpool == NULL)
	{
		return NULL;
	}
	
	worker->tid = pthread_self();

	tpool_index = worker->tpool_index;
	/*LOG_TRACE_NORMAL("server tpool worker"
			" [tpool_index=%d] start ...\n", tpool_index);*/

	event_service_working_count(1);

	event_base_dispatch(worker->evbase);
	event_del(&worker->event);
	event_base_free(worker->evbase);
	worker->evbase = NULL;

	event_service_working_count(-1);
	
	LOG_TRACE_NORMAL("server tpool worker"
			" [tpool_index=%d] exit ...\n", tpool_index);
	
	return NULL;
}

bool event_worker_add_job(
		event_worker_t *worker, 
		void *arg, 
		void (*free_arg_cb)(void *), 
		unsigned int timer_out, 
		executor_cb executor_cb, 
		event_cb_fn timer_cb, 
		bufferevent_data_cb read_cb, 
		bufferevent_data_cb write_cb,
	    bufferevent_event_cb error_cb)
{
	event_worker_job_t job;

	if (worker == NULL 
		|| arg == NULL 
		|| worker->write_fd <= 0 
		|| worker->queue == NULL)
	{
		return false;
	}

	pthread_mutex_lock(&worker->lock);

	_event_worker_job_init(
			&job, arg, free_arg_cb, timer_out, executor_cb, timer_cb, 
			read_cb, write_cb, 
			error_cb);

	item_t *item = item_alloc(worker->queue, 
				&job, sizeof(event_worker_job_t));
	if (item == NULL)
	{
		LOG_TRACE_NORMAL("item_alloc error!\n");
		pthread_mutex_unlock(&worker->lock);
		return false;
	}
	
	if (queue_push(worker->queue, item) < 0)
	{
		LOG_TRACE_NORMAL("queue_push error!\n");
		item_free(worker->queue, item);
		pthread_mutex_unlock(&worker->lock);
		return false;
	}

	event_service_job_wait_count(1);

	send(worker->write_fd, " ", 1, 0);

	pthread_mutex_unlock(&worker->lock);
	return true;
}



