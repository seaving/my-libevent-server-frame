#ifndef __SYS_INC_H__
#define __SYS_INC_H__

#include <arpa/inet.h>
#include <alloca.h>

#include <ctype.h>

#include <dirent.h>
#include <dlfcn.h>

#include <errno.h>
#include <fcntl.h>

#include <limits.h>
#include <libgen.h>
#include <linux/sockios.h>

#include <math.h>

#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#include <resolv.h>
#include <time.h> 

#include <termios.h>

#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

#include <signal.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <sys/time.h>

/*
	必须定义宏，并且必须在stdio.h之后定义，
	并且pthread.h必须在sched.h之后
*/
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
/************* end ***********************/

#include <json.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/ec.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <event2/dns.h>


#ifdef __ANDROID_PLATFORM__
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/vfs.h>
#include <sys/system_properties.h>
#include <stdint.h>
#include <sys/inotify.h>
#include <android/log.h>
#include <sys/mount.h>
#include <sys/limits.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define pthread_spinlock_t		pthread_mutex_t
#define pthread_spin_lock 		pthread_mutex_lock
#define pthread_spin_unlock		pthread_mutex_unlock

#define pthread_spin_init(l, m) pthread_mutex_init(l, NULL)
#define pthread_spin_destroy	pthread_mutex_destroy

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

#endif

#endif

