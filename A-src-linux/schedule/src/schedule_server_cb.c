#include "includes.h"

bool schedule_server_user_data_init_cb(talk_user_data_t *user_data)
{
    cli_phone_t *cli_phone = cli_phone_create();
    if (cli_phone == NULL)
    {
        return false;
    }

    user_data->arg = (void *) cli_phone;
    user_data->free_arg = cli_phone_free;

	return true;
}

bool schedule_server_timer_cb(evexecutor_t executor, event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	return true;
}

bool schedule_server_read_cb(evexecutor_t executor, event_buf_t *event_buf, int conn_fd, void *arg)
{
	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf && cli_phone)
	{
		if (cli_phone_talk(executor, event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
		{
			return false;
		}
	}

	return true;
}

bool schedule_server_write_cb(evexecutor_t executor, event_buf_t *event_buf, int conn_fd, void *arg)
{
	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf && cli_phone)
	{
		if (cli_phone_talk(executor, event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
		{
			return false;
		}
	}

	return true;
}

bool schedule_server_error_cb(evexecutor_t executor, event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	return true;
}

