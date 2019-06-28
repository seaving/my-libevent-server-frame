#include "sys_inc.h"
#include "log_trace.h"

bool rlimit_set_file_size()
{
	struct rlimit rlim;
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;

	if (setrlimit(RLIMIT_FSIZE, &rlim) < 0)
	{
		LOG_TRACE_PERROR("setrlimit error !\n");
		return false;
	}

	return true;
}


