#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/data/data/com.rmgoncalo.droidg/dg_daemon_server"
#define SOCKJAVA_PATH "/abstract/usocket"

#define PORT_SIZE 2
#define INT_SIZE 4
#define PROCESS_SIZE 4096

#define DG_INET 1
#define DG_INET6 2

#define UNDEFINED -1
#define DENY 0
#define ALLOW 1

#define ONCE 0
#define FOREVER 1

#define ALLOW_FOREVER 1
#define DENY_FOREVER 2
#define ALLOW_ONCE 3
#define DENY_ONCE 4

struct dg_query{
	int family;
	struct sockaddr_in addrin;
	struct sockaddr_in6 addrin6;
	int pid;
	int permission;
	int lifetime;
};
