#include "sys_inc.h"
#include "log_trace.h"
#include "frame.h"
#include "fifo.h"

#define FRAME_TOKEN		"---&&88###@@9900332ysaadsd"
//--------------------------------------------------------------------------------------------------------------------
#define check_frame_head(head)	\
	(head == NULL ? false : strncmp((char *) head, TUNNEL_COM_FRAME_HEAD, TUNNEL_COM_FRAME_FIELD_HEAD_LEN) == 0)
//--------------------------------------------------------------------------------------------------------------------
#define check_frame_end(end)	\
	(end == NULL ? false : strncmp((char *) end, TUNNEL_COM_FRAME_END, TUNNEL_COM_FRAME_FIELD_END_LEN) == 0)
//--------------------------------------------------------------------------------------------------------------------
static unsigned int _cal_sum(unsigned char *frame, unsigned int frame_len)
{
	int i = 0;
	unsigned int sum = 0;
	char *token = FRAME_TOKEN;
	int token_len = strlen(token);
	if (frame == NULL 
		|| frame_len <= 2)
	{
		return 0;
	}

	for (i = 0; i < frame_len - 2; i ++)
	{
		sum += frame[i];
	}

	for (i = 0; i < token_len; i ++)
	{
		sum += token[i];
	}

	return sum;
}

static bool _frame_data_process(int cmd, unsigned char *frame_data, int datalen)
{
	if (frame_data == NULL 
		|| datalen <= 0 
		|| datalen > 128)
	{
		return false;
	}

	switch (cmd)
	{
		case TUNNEL_COM_FRAME_CMD_TRANS_CREATE:
			break;
		case TUNNEL_COM_FRAME_CMD_SOCKET_CREATE:
			break;
		case TUNNEL_COM_FRAME_CMD_TRANS_DESTROY:
			break;
		case TUNNEL_COM_FRAME_CMD_SOCKET_DESTROY:
			break;
		case TUNNEL_COM_FRAME_CMD_HEART:
			break;
		default:
		{
			LOG_TRACE_NORMAL("Unkown cmd: 0x%04X\n", cmd);
			return false;
		}
	}

	return true;
}

void frame_parse(fifo_t *fifo)
{
	int n = 0;
	int cmd = 0;
	int offset = 0;
	int datalen = 0;
	unsigned int sum = 0;
	unsigned char buffer[128] = {0};

	if (fifo == NULL)
	{
		return;
	}

	for ( ; fifo && FIFO_DATA_LEN(fifo) > 0; )
	{
		offset = 0;

		//头部读取
		n = fifo_pre_read(fifo, 0, buffer + offset, TUNNEL_COM_FRAME_FIELD_HEAD_LEN);
		if (n != TUNNEL_COM_FRAME_FIELD_HEAD_LEN)
		{
			//fifo中数据量不够
			break;
		}

		//判断是不是头部
		if (check_frame_head(buffer + offset) == false)
		{
			//不是头部，移到下一位
			fifo_read(fifo, buffer, 1);
			continue;
		}		
		offset += n;

		//读取命令字段
		n = fifo_pre_read(fifo, offset, buffer + offset, TUNNEL_COM_FRAME_FIELD_CMD_LEN);
		if (n != TUNNEL_COM_FRAME_FIELD_CMD_LEN)
		{
			//fifo中数据量不够
			break;
		}

		cmd = buffer[offset] << 8 | buffer[offset + 1];
		offset += n;

		//读取数据长度字段
		n = fifo_pre_read(fifo, offset, buffer + offset, TUNNEL_COM_FRAME_FIELD_DATALEN_LEN);
		if (n != TUNNEL_COM_FRAME_FIELD_DATALEN_LEN)
		{
			//fifo中数据量不够
			break;
		}

		//判断长度是否合法
		datalen = buffer[offset] << 8 | buffer[offset + 1];
		if (datalen > 128)
		{
			//长度异常，移到下一位
			fifo_read(fifo, buffer, 1);
			continue;
		}
		offset += n;

		//读取数据
		n = fifo_pre_read(fifo, offset, buffer + offset, datalen);
		if (n != datalen)
		{
			//fifo中数据量不够
			break;
		}
		offset += n;

		//读取校验和
		n = fifo_pre_read(fifo, offset, buffer + offset, TUNNEL_COM_FRAME_FIELD_CHECKSUM_LEN);
		if (n != TUNNEL_COM_FRAME_FIELD_CHECKSUM_LEN)
		{
			//fifo中数据量不够
			break;
		}

		sum = buffer[offset] << 24 
			| buffer[offset + 1] << 16 
			| buffer[offset + 2] << 8 
			| buffer[offset + 3];
		offset += n;

		//读取结尾
		n = fifo_pre_read(fifo, offset, buffer + offset, TUNNEL_COM_FRAME_FIELD_END_LEN);
		if (n != TUNNEL_COM_FRAME_FIELD_END_LEN)
		{
			//fifo中数据量不够
			break;
		}

		//判断是不是结尾
		if (check_frame_end(buffer + offset) == false)
		{
			//不是头部，移到下一位
			fifo_read(fifo, buffer, 1);
			continue;
		}
		offset += n;

		//判断校验和是否正确
		if (sum != _cal_sum(buffer, offset))
		{
			//校验和错误，移到下一位
			fifo_read(fifo, buffer, 1);
			continue;
		}

		//获取到完整帧，分析完整帧
		offset = TUNNEL_COM_FRAME_FIELD_HEAD_LEN 
					+ TUNNEL_COM_FRAME_FIELD_CMD_LEN 
					+ TUNNEL_COM_FRAME_FIELD_DATALEN_LEN;
		if (_frame_data_process(cmd, &buffer[offset], datalen) == false)
		{
			//帧数据段解析失败，移到下一位
			fifo_read(fifo, buffer, 1);
			continue;
		}

		//解析成功，从fifo中删除该帧
		fifo_read(fifo, buffer, offset);
	}
}



