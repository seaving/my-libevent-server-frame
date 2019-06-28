#ifndef __SOCKET_H__
#define __SOCKET_H__


int socket_setfd_noblock(int sockfd);
int socket_setfd_block(int sockfd);
int get_addr_info(char *hostname, char *ipaddr);
void socket_close(int socket);

int socket_listen(char *bindip, int listen_port, int listen_counts);

#endif


