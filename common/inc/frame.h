#ifndef __FRAME_H__
#define __FRAME_H__


#include "fifo.h"

/*
格式:
	6 byte     2byte	     2byte      datalen   4byte         2byte
	head        cmd         datalen      data    checksum       end
	;;##!!   0x00 0x01     0x00 0x00      ...    head+cmd		\r\n
											   	+datalen
											     +data
											     +token求和
*/

#define TUNNEL_COM_FRAME_HEAD					";;##!!"
#define TUNNEL_COM_FRAME_END					"\r\n"

#define TUNNEL_COM_FRAME_FIELD_HEAD_LEN			6
#define TUNNEL_COM_FRAME_FIELD_CMD_LEN			2
#define TUNNEL_COM_FRAME_FIELD_DATALEN_LEN		2
#define TUNNEL_COM_FRAME_FIELD_CHECKSUM_LEN		4
#define TUNNEL_COM_FRAME_FIELD_END_LEN			2

#define TUNNEL_COM_FRAME_CMD_TRANS_CREATE		0x0001		//建立透传隧道
#define TUNNEL_COM_FRAME_CMD_SOCKET_CREATE		0x0002		//建立第三方服务器socket连接
#define TUNNEL_COM_FRAME_CMD_TRANS_DESTROY		0x0003		//销毁透传隧道
#define TUNNEL_COM_FRAME_CMD_SOCKET_DESTROY		0x0004		//销毁socket
#define TUNNEL_COM_FRAME_CMD_HEART				0x0005		//心跳


void frame_parse(fifo_t *fifo);




#endif


