#include "includes.h"

static void _signal_worker_exit(int signal)
{
	LOG_TRACE_NORMAL("WARN: worker exit !!!\n");

	int i = 0;
	int error = 0;
	int status = 0;
	pid_t pid = -1;

	uintptr_t one = 0;

	char *process = NULL;

	for ( ; ; )
	{
		pid = waitpid(-1, &status, WNOHANG);
		if (pid == 0)
		{
			return;
		}

		if (pid == -1)
		{
			error = errno;
			if (error == EINTR)
			{
				continue;
			}

			if (error == ECHILD && one)
			{
				return;
			}

			if (error == ECHILD)
			{
				LOG_TRACE_PERROR("waitpid() error !\n");
				return;
			}

			LOG_TRACE_PERROR("waitpid() error !\n");

			return;
		}

		one = 1;
		process = "unknown process";

		for (i = 0; i < sch_master.worker_num; i ++)
		{
			if (sch_master.workers[i].pid == pid)
			{
				sch_master.workers[i].status = status;
				sch_master.workers[i].exited = 1;
				process = sch_master.workers[i].name;
				break;
			}
		}

		if (WTERMSIG(status))
		{
#ifdef __COREDUMP__
			LOG_TRACE_NORMAL("the worker (pid: %d name:%s) exited on signal %d%s !\n", 
				pid, process, WTERMSIG(status), WCOREDUMP(status) ? " (core dumped)" : "");
#else
			LOG_TRACE_NORMAL("the worker (pid: %d name:%s) exited on signal %d !\n", 
				pid, process, WTERMSIG(status));
#endif
		}
		else
		{
			LOG_TRACE_NORMAL("the worker (pid: %d name:%s) exited with code %d !\n", 
				pid, process, WEXITSTATUS(status));
		}

		if (WEXITSTATUS(status) == 2 && sch_master.workers[i].respawn)
		{
			LOG_TRACE_NORMAL("the worker (pid: %d name:%s) exited with fatal code %d "
				"and cannot be respawned!\n", pid, process, WEXITSTATUS(status));
		}
	}
}

static void _signal_interrupt_exit(int signal)
{
	LOG_TRACE_NORMAL("WARN: worker exit !!!\n");
	schedule_server_stop();
	exit(0);
}

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, _signal_worker_exit);
	signal(SIGINT, _signal_interrupt_exit);

	time_init();

	if (schedule_master_init() == false)
    {
        LOG_TRACE_NORMAL("schedule_master_init error !\n");
        return -1;
    }

	if (schedule_server_init() == false)
	{
		LOG_TRACE_NORMAL("schedule_server_init error!\n");
		return -1;
	}

	schedule_master_create_workers();

	LOG_TRACE_NORMAL("\n");
	LOG_TRACE_NORMAL("------ father loop start ------\n");

	for ( ; ; )
	{
		sleep(1);
	}

	return 0;
}

