#include "includes.h"

cli_http_t *cli_http_create()
{
    cli_http_t *cli_http = calloc(sizeof(cli_http_t), 1);
    if (cli_http == NULL)
    {
        LOG_TRACE_PERROR("calloc error !\n");
        return NULL;
    }

    cli_http->step = E_CLI_HTTP_STEP_SEND_REQUEST;
    return cli_http;
}

void cli_http_free(void *cli)
{
	cli_http_t *cli_http = (cli_http_t *) cli;
	if (cli_http)
	{
		if (cli_http->proxy_valid
			&& cli_http->cli_phone)
		{
			cli_http->cli_phone->proxy_valid = false;
			cli_http->cli_phone->cli_http = NULL;
			cli_http->proxy_valid = NULL;
			cli_http->cli_phone = NULL;
			cli_http->event_buf = NULL;
		}

		httpRequest_context_free(&cli_http->httpRequest);
		memset(cli_http, 0, sizeof(cli_http_t));
		free(cli_http);
	}
}

