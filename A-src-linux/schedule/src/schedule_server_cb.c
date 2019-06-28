#include "includes.h"

bool schedule_server_user_data_init_cb(talk_user_data_t *user_data)
{
	//LOG_TRACE_NORMAL("schedule_server_user_data_init_cb\n");

    cli_phone_t *cli_phone = cli_phone_create();
    if (cli_phone == NULL)
    {
        return false;
    }

    user_data->arg = (void *) cli_phone;
    user_data->free_arg = cli_phone_free;

	return true;
}

bool schedule_server_timer_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("schedule_server_timer_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	return true;
}

bool schedule_server_read_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("schedule_server_read_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	if (cli_phone_talk(event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
	{
		return false;
	}

	return true;
}

bool schedule_server_write_cb(event_buf_t *event_buf, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("schedule_server_write_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	if (cli_phone_talk(event_buf, cli_phone) == E_CLI_PHONE_TALK_FINISHED)
	{
		return false;
	}

	return true;
}

bool schedule_server_error_cb(event_buf_t *event_buf, short what, int conn_fd, void *arg)
{
	//LOG_TRACE_NORMAL("schedule_server_error_cb\n");

	cli_phone_t *cli_phone = (cli_phone_t *) arg;
	if (event_buf == NULL 
		|| cli_phone == NULL)
	{
		return false;
	}

	return true;
}

