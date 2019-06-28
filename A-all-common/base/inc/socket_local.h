#ifndef __SOCKET_LOCAL_H
#define __SOCKET_LOCAL_H

#include <sys/un.h>

typedef int cutils_socket_t;
#define INVALID_SOCKET (-1)


#define ANDROID_SOCKET_ENV_PREFIX	"ANDROID_SOCKET_"
#define ANDROID_SOCKET_DIR			"/dev/socket"


#define FILESYSTEM_SOCKET_PREFIX 		"/tmp/" 
#define ANDROID_RESERVED_SOCKET_PREFIX 	"/dev/socket/"

/*
 * See also android.os.LocalSocketAddress.Namespace
 */
// Linux "abstract" (non-filesystem) namespace
#define ANDROID_SOCKET_NAMESPACE_ABSTRACT 	0
// Android "reserved" (/dev/socket) namespace
#define ANDROID_SOCKET_NAMESPACE_RESERVED 	1
// Normal filesystem namespace
#define ANDROID_SOCKET_NAMESPACE_FILESYSTEM 2

/** Open a server-side UNIX domain datagram socket in the Linux non-filesystem 
 *	namespace
 *
 *	Returns fd on success, -1 on fail
 */
int socket_local_server(const char *name, int namespace, int listen_count);

int socket_local_socket_create();

/* Documented in header file. */
int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);


#endif

