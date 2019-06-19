#include "includes.h"

cli_phone_t *cli_phone_create()
{
    cli_phone_t *cli_phone = calloc(sizeof(cli_phone_t), 1);
    if (cli_phone == NULL)
    {
        LOG_TRACE_PERROR("calloc error !\n");
        return NULL;
    }

    cli_phone->step = E_CLI_PHONE_STEP_READ_HEADER;
    return cli_phone;
}

void cli_phone_free(void *cli)
{
	cli_phone_t *cli_phone = (cli_phone_t *) cli;
	if (cli_phone)
	{
		if (cli_phone->free_proxy)
		{
			cli_phone->free_proxy(cli_phone->proxy);
		}

		httpRequest_context_free(&cli_phone->httpRequest);
		memset(cli_phone, 0, sizeof(cli_phone_t));
		free(cli_phone);
	}
}

