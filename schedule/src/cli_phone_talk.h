#ifndef __CLI_PHONE_TALK_H__
#define __CLI_PHONE_TALK_H__

#include "event_executor.h"
#include "cli_phone.h"

typedef enum __cli_phone_talk_result__
{
	E_CLI_PHONE_TALK_CONTINUE,
	E_CLI_PHONE_TALK_FINISHED
} cli_phone_talk_result_t;

cli_phone_talk_result_t cli_phone_talk(event_buf_t *event_buf, cli_phone_t *cli_phone);

#endif

