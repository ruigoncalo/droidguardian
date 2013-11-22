#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"
#define PROCESS_SIZE 16

struct dg_query {
  	int server;
	int port;
	int pid;
	char process[PROCESS_SIZE];

};

#define dg_query_size sizeof(struct dg_query)

int recv_query_from_module(int sockfd, struct dg_query *query);
ssize_t send_answer_to_module(int sockfd, struct dg_query *query);


