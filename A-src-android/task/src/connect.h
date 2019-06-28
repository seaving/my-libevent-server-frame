#ifndef __CONNECT_H__
#define __CONNECT_H__


/*
* 函数: connect_server
* 功能: 连接服务器
* 参数: 无
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_server();

/*
* 函数: connect_server_is_ok
* 功能: 连接服务器状态
* 参数: 无
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_server_is_ok();

/*
* 函数: connect_send_data
* 功能: 发送数据到服务器
* 参数: data        数据
*       datalen     长度
* 返回: bool
*       - false     失败
* 说明: 
*/
bool connect_send_data(char *data, int data_len);

#endif


