#ifndef __CLI_PHONE_H__
#define __CLI_PHONE_H__

#include "http_request.h"

typedef enum __cli_phone_step__
{
	E_CLI_PHONE_STEP_READ_HEADER = 0,
	E_CLI_PHONE_STEP_CHECK_HEADER,
	E_CLI_PHONE_STEP_FRAME_PARSE,
	E_CLI_PHONE_STEP_READ_CONTEXT,
	E_CLI_PHONE_STEP_SEND_SUCCESS,
	E_CLI_PHONE_STEP_SEND_ERROR,
	E_CLI_PHONE_STEP_FINISHED
} cli_phone_step_t;

typedef struct __cli_phone__
{
    httpRequest_t httpRequest;
    cli_phone_step_t step;
} cli_phone_t;

cli_phone_t *cli_phone_create();
void cli_phone_free(void *cli);

#endif


