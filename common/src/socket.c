#include "sys_inc.h"
#include "log_trace.h"
#include "user_time.h"

int socket_setfd_noblock(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_TRACE_PERROR("fcntl F_GETFL error!\n");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag | O_NONBLOCK)) < 0)
	{
		LOG_TRACE_PERROR("fcntl F_SETFL error!\n");
		return -1;
	}
	
	return 0;
}

int socket_setfd_block(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_TRACE_PERROR("fcntl F_GETFL error!\n");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag & ~O_NONBLOCK)) < 0)
	{
		LOG_TRACE_PERROR("fcntl F_SETFL error!\n");
		return -1;
	}
	
	return 0;
}

int get_addr_info(char *hostname, char *ipaddr)
{  
	struct addrinfo *answer = NULL, hint/*, *curr*/;
	char *ipstr = ipaddr;
	struct sockaddr_in *addr;
	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_INET;//AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	int ret = -1;

	long last_time = 0L;
	long cur_time = GetTimestrip_us() / 1000000;
	last_time = cur_time;
	
	while (1)
	{
		cur_time = GetTimestrip_us() / 1000000;
		if (cur_time - last_time > 10)
		{
			LOG_TRACE_NORMAL("[ %s ] getaddrinfo timeout !\n", hostname);
			if (answer)
				freeaddrinfo(answer);
			return -1;
		}
		ret = getaddrinfo(hostname, NULL, &hint, &answer);
		if (ret != 0)
		{
			LOG_TRACE_NORMAL("[ %s ] getaddrinfo: ret = %d, %s\n", 
					hostname, ret, gai_strerror(ret));
			if (ret == EAI_AGAIN)
				continue;
			if (answer)
				freeaddrinfo(answer);
			return -1;
		}
		break;
	}

	//for (curr = answer; curr != NULL; curr = curr->ai_next)
	{
		if (! answer)
			return -1;

		addr = (struct sockaddr_in *)answer->ai_addr;
		inet_ntop(AF_INET, &addr->sin_addr, ipstr, 16);
		//break;
	}

	freeaddrinfo(answer);

	return 0;
}

void socket_close(int socket)
{
	if (socket > 0)
	{
		close(socket);
	}
}

int socket_listen(char *bindip, int listen_port, int listen_counts)
{
    struct sockaddr_in server_addr;
    int listenfd;
    int ret = -1;
    
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port);
    if (bindip == NULL)
    {
    	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
		server_addr.sin_addr.s_addr = inet_addr(bindip);
    }
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd <= 0)
    {
        LOG_TRACE_PERROR("create socket error!\n");
        return -1;
    }
    
    int len = sizeof(struct sockaddr);
    ret = bind(listenfd, (struct sockaddr *) &server_addr, len);
    if (ret == -1)
    {
        LOG_TRACE_PERROR("bind error, "
        	"lisent_fd=%d, port=%d!\n", listenfd, listen_port);
        socket_close(listenfd);
        return -1;
    }
    
    ret = listen(listenfd, listen_counts);
    if (ret == -1)
    {
        LOG_TRACE_PERROR("listen error, "
        	"listenfd=%d, listen_counts=%d!\n", listenfd, listen_counts);
        socket_close(listenfd);
        return -1;
    }
    
    return listenfd;
}

