#include "sys_inc.h"
#include "log_trace.h"
#include "socket.h"
#include "socket_local.h"

/* Only the bottom bits are really the socket type; there are flags too. */
#define SOCK_TYPE_MASK 0xf

#define UNUSED __attribute__((unused))

static int _exists(const char *path)
{
    return ! access(path, R_OK);
}

static int _setxattr(const char *name, const char *value)
{
    char path[128] = {0};
    if (! _exists("/sys/fs/selinux"))
    {
        return 0;
    }

    strcpy(path, ANDROID_RESERVED_SOCKET_PREFIX);
    strcat(path, name);

    return syscall(__NR_setxattr, path, "security.selinux", value, strlen(value), 0);
}

/* Documented in header file. */
int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen)
{
    memset (p_addr, 0, sizeof (*p_addr));
    size_t namelen;

    switch (namespaceId) {
        case ANDROID_SOCKET_NAMESPACE_ABSTRACT:
#if defined(__linux__)
            namelen  = strlen(name);

            // Test with length +1 for the *initial* '\0'.
            if ((namelen + 1) > sizeof(p_addr->sun_path)) {
                goto error;
            }

            /*
             * Note: The path in this case is *not* supposed to be
             * '\0'-terminated. ("man 7 unix" for the gory details.)
             */
            
            p_addr->sun_path[0] = 0;
            memcpy(p_addr->sun_path + 1, name, namelen);
#else
            /* this OS doesn't have the Linux abstract namespace */

            namelen = strlen(name) + strlen(FILESYSTEM_SOCKET_PREFIX);
            /* unix_path_max appears to be missing on linux */
            if (namelen > sizeof(*p_addr) 
                    - offsetof(struct sockaddr_un, sun_path) - 1) {
                goto error;
            }

            strcpy(p_addr->sun_path, FILESYSTEM_SOCKET_PREFIX);
            strcat(p_addr->sun_path, name);
#endif
        break;

        case ANDROID_SOCKET_NAMESPACE_RESERVED:
            namelen = strlen(name) + strlen(ANDROID_RESERVED_SOCKET_PREFIX);
            /* unix_path_max appears to be missing on linux */
            if (namelen > sizeof(*p_addr) 
                    - offsetof(struct sockaddr_un, sun_path) - 1) {
                goto error;
            }

            strcpy(p_addr->sun_path, ANDROID_RESERVED_SOCKET_PREFIX);
            strcat(p_addr->sun_path, name);
        break;

        case ANDROID_SOCKET_NAMESPACE_FILESYSTEM:
            namelen = strlen(name);
            /* unix_path_max appears to be missing on linux */
            if (namelen > sizeof(*p_addr) 
                    - offsetof(struct sockaddr_un, sun_path) - 1) {
                goto error;
            }

            strcpy(p_addr->sun_path, name);
        break;
        default:
            // invalid namespace id
            return -1;
    }

    p_addr->sun_family = AF_LOCAL;
    *alen = namelen + offsetof(struct sockaddr_un, sun_path) + 1;
    return 0;
error:
    return -1;
}

int socket_local_server_bind(int s, const char *name, int namespaceId)
{
    struct sockaddr_un addr;
    socklen_t alen;
    int n;
    int err;

    err = socket_make_sockaddr_un(name, namespaceId, &addr, &alen);

    if (err < 0) {
        return -1;
    }

    /* basically: if this is a filesystem path, unlink first */
#if !defined(__linux__)
    if (1) {
#else
    if (namespaceId == ANDROID_SOCKET_NAMESPACE_RESERVED
        || namespaceId == ANDROID_SOCKET_NAMESPACE_FILESYSTEM) {
#endif
        /*ignore ENOENT*/
        unlink(addr.sun_path);
    }

    n = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));

    if(bind(s, (struct sockaddr *) &addr, alen) < 0) {
        return -1;
    }

    return s;

}

/** 
 * connect to peer named "name"
 * returns fd or -1 on error
 */
int socket_local_socket_create()
{
	int s;

	s = socket(AF_LOCAL, SOCK_STREAM, 0);
	if(s < 0)
	{
		LOG_TRACE_ERROR("socekt error !\n");
		return -1;
	}

	return s;
}

/** Open a server-side UNIX domain datagram socket in the Linux non-filesystem 
 *	namespace
 *
 *	Returns fd on success, -1 on fail
 */
int socket_local_server(const char *name, int namespace, int listen_count)
{
	int err;
	int s;
	
	s = socket_local_socket_create();

	err = socket_local_server_bind(s, name, namespace);

	if (err < 0) {
		close(s);
		return -1;
	}

    if (listen(s, listen_count) == -1)
    {
        LOG_TRACE_PERROR("listen error, "
        	"listenfd=%d, listen_counts=%d!\n", s, listen_count);
        close(s);
        return -1;
    }

    _setxattr(name, "u:object_r:dnsproxyd_socket:s0");

	return s;
}

